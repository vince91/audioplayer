//
//  audiofile.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#include <portaudio.h>
#include <fstream>
#include <thread>
#include <cmath>
#include "audiofile.h"
#include "waveform.h"
#include "mainwindow.h"
#include <QDir>

#define SHORT_MAX 32768.
#define THREAD_STOP_REQUESTED -2


AudioFile::AudioFile(MainWindow* _window, std::string _filename) :  window(_window), filename(_filename)
{
    av_register_all();
}

AudioFile::~AudioFile()
{
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);
    avformat_close_input(&waveformFormatContext);
    av_frame_free(&frame);
    av_frame_free(&waveformFrame);
    
    if (waveformThread != nullptr) {
        if (waveformThread->joinable())
            waveformThread->join();
        delete waveformThread;
    }
}

bool AudioFile::initialize()
{
    
    /* open input file, and allocate format context */
    if (avformat_open_input(&formatContext, filename.c_str(), NULL, NULL) < 0) {
        std::cerr << "Could not open source file (" << filename << ")\n";
        return false;
    }
    
    /* retrieve stream information */
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        std::cerr << "Could not find stream information (" << filename << ")\n";
        return false;
    }
    
    
    /* open the apropriate audio codec */
    if (!openCodecContext(formatContext, audioStreamIndex, audioStream, codecContext)) {
        std::cerr << "Could not open audio codec\n" << std::endl;
        return false;
    }
    
    
    
    /* dump file info to stderr */
    av_dump_format(formatContext, 0, filename.c_str(), 0);
    
    waveformThread = new std::thread(&AudioFile::createWaveform, this);
    
    /* basic informations: metadata, sample format and mono/stereo */
    retrieveMetadata();
    saveAlbumCover();
    sampleFormat = codecContext->sample_fmt;
    if (codecContext->channels == 2)
        stereo = true;
    
    /* audio decoding */
    frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Could not allocate frame\n";
        return false;
    }
    
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
    
    fillBuffer(); // fill the first buffer part
    
    return true;
}

bool AudioFile::openCodecContext(AVFormatContext *fmtContext, int &streamIndex, AVStream * &stream, AVCodecContext * &cdcContext)
{
    AVCodec *codec = NULL;
    
    streamIndex = av_find_best_stream(fmtContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    
    if (streamIndex < 0) {
        std::cerr << "Could not find audio stream in input file " << filename << std::endl;
        return false;
    }
    
    stream = fmtContext->streams[streamIndex];
    
    /* find decoder for the stream */
    cdcContext = stream->codec;
    codec = avcodec_find_decoder(cdcContext->codec_id);
    
    if (!codec) {
        std::cerr << "Failed to find audio codec\n";
        return false;
    }
    
    /* Init the decoders without reference counting */
    if (avcodec_open2(cdcContext, codec, NULL) < 0) {
        std::cerr << "Failed to open audio codec\n";
        return false;
    }
    
    return true;
    
}


int AudioFile::decodePacket()
{
    int ret = 0;
    int decoded = packet.size;
    gotFrame = 0;
    
    /* decode audio frame */
    ret = avcodec_decode_audio4(codecContext, frame, &gotFrame, &packet);
    
    if (ret < 0) {
        fprintf(stderr, "Error decoding audio frame (%s)\n", av_err2str(ret));
        return ret;
    }
    
    decoded = FFMIN(ret, packet.size);
    
    if (gotFrame) {
        
        for (int i = 0; i < frame->nb_samples; ++i) {
            
            if(sampleFormat == AV_SAMPLE_FMT_S16P) {
                firstChannel[writePos] = (short) (frame->extended_data[0][2 * i] | frame->extended_data[0][2 * i + 1] << 8) / SHORT_MAX;
                
                if (stereo)
                    secondChannel[writePos] = (short) (frame->extended_data[1][2 * i] | frame->extended_data[1][2 * i + 1] << 8) / SHORT_MAX;
                

                
            }
            else if (sampleFormat == AV_SAMPLE_FMT_S16) {
                firstChannel[writePos] = (short) (frame->extended_data[0][4 * i] | frame->extended_data[0][4 * i + 1] << 8) / SHORT_MAX;
                
                if (stereo)
                    secondChannel[writePos] = (short) (frame->extended_data[0][4 * i + 2] | frame->extended_data[0][4 * i + 3] << 8) / SHORT_MAX;
                


                
                
                
            }
            
            if(++writePos == 3*BUFFER_SIZE)
                writePos = 0;

            // av_samples_fill_array
            
        }
        
    }
    
    return decoded;
    
}

bool AudioFile::fillBuffer()
{
    int lenght;
    int count = 0;
    
    /* read frames from the file */
    while (count < BUFFER_SIZE && av_read_frame(formatContext, &packet) >= 0) {
        AVPacket orig_pkt = packet;
        do {
            lenght = decodePacket();
            count += frame->nb_samples;
            packet.data += lenght;
            packet.size -= lenght;
        } while (packet.size > 0 && lenght >= 0);
        av_free_packet(&orig_pkt);
    }
    
    if (count < BUFFER_SIZE) {
        /* flush cached frames */
        packet.data = NULL;
        packet.size = 0;
        do {
            decodePacket();
            count +=  frame->nb_samples;
        } while (gotFrame && count < BUFFER_SIZE);
    }
    
    bool ret = (count >= BUFFER_SIZE);
    
    if (!ret)
        lastIndex = writePos;
    
    return ret; /* returns false when entire audio file has been read */
}

void AudioFile::threadFillBuffer()
{
    /* constantly fill circular buffer so portaudio has always enough data to process */
    while (lastIndex != THREAD_STOP_REQUESTED) {
        
        if (seekRequested) {
            
        }
        else {
            if (writePos > readPos) {
                if ((writePos - readPos - 1) < BUFFER_SIZE)
                    fillBuffer();
            }
            else {
                if ((writePos + 3*BUFFER_SIZE - readPos) < BUFFER_SIZE)
                    fillBuffer();
            }
        }
        
        window->updateTime(playedSamples/44100.);
        Pa_Sleep(10);
    }
}

void AudioFile::retrieveMetadata()
{
    AVDictionary *dictionary = formatContext->metadata; AVDictionaryEntry *tag;
    tag = av_dict_get(dictionary, "title", NULL, AV_DICT_MATCH_CASE);
    if (tag != NULL)
        title = tag->value;
    tag = av_dict_get(dictionary, "artist", NULL, AV_DICT_MATCH_CASE);
    if (tag != NULL)
        artist = tag->value;
    tag = av_dict_get(dictionary, "album", NULL, AV_DICT_MATCH_CASE);
    if (tag != NULL)
        album = tag->value;
    tag = av_dict_get(dictionary, "date", NULL, AV_DICT_MATCH_CASE);
    if (tag != NULL)
        year = tag->value;
    tag = av_dict_get(dictionary, "genre", NULL, AV_DICT_MATCH_CASE);
    if (tag != NULL)
        genre = tag->value;
    
    int64_t d = formatContext->duration/AV_TIME_BASE;
    duration = ((d/60<10)?"0":"") + std::to_string(d/60) + ":" + ((d%60<10)?"0":"") + std::to_string(d%60);
}

void AudioFile::saveAlbumCover()
{
    /* save album cover to temp folder */
    videoStreamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    std::string file = QDir::tempPath().toStdString() + "/audio_player_cover";
    
    if (videoStreamIndex >= 0) {
        videoStream = formatContext->streams[videoStreamIndex];
        
        if (videoStream != NULL) {
            if (videoStream->disposition & AV_DISPOSITION_ATTACHED_PIC) {
                std::ofstream outFile(file, std::ofstream::binary);
                AVPacket pkt = videoStream->attached_pic;
                outFile.write((char*)pkt.data, pkt.size);
                outFile.close();
                return;
            }
        }
    }
    std::remove(file.c_str()); // delete the file is there isn't any album cover
}

bool AudioFile::createWaveform()
{
    //std::cerr << "Waveform calculation\n";
    
    if (avformat_open_input(&waveformFormatContext, filename.c_str(), NULL, NULL) < 0) {
        std::cerr << "Waveform: could not open source file (" << filename << ")\n";
        return false;
    }
    
    if (avformat_find_stream_info(waveformFormatContext, NULL) < 0) {
        std::cerr << "Waveform: could not find stream information (" << filename << ")\n";
        return false;
    }
    
    if (!openCodecContext(waveformFormatContext, audioStreamIndex, audioStream, waveformCodecContext)) {
        std::cerr << "Waveform: ould not open audio codec\n" << std::endl;
        return false;
    }
    
    waveformFrame = av_frame_alloc();
    if (!waveformFrame) {
        std::cerr << "Waveform: could not allocate frame\n";
        return false;
    }
    
    av_init_packet(&waveformPacket);
    waveformPacket.data = NULL;
    waveformPacket.size = 0;
    
    waveform = new Waveform();
    samplesPerChunk = (float) waveformFormatContext->duration/AV_TIME_BASE * codecContext->sample_rate / WAVEFORM_SIZE;
    waveformBufferSize = (samplesPerChunk < 10000)?10000:samplesPerChunk;
    
    waveformBuffer = new float[3*waveformBufferSize];
    
    int lenght;
    int count = 0;
    
    /* read frames from the file */
    while (av_read_frame(waveformFormatContext, &waveformPacket) >= 0) {
        AVPacket orig_pkt = waveformPacket;
        do {
            lenght = waveformDecodePacket();
            count += waveformFrame->nb_samples;
            if (lenght >= 0) {
                waveformPacket.data += lenght;
                waveformPacket.size -= lenght;
            }
        } while (waveformPacket.size > 0 && lenght >= 0);
        av_free_packet(&orig_pkt);
    }
    
    /* flush cached frames */
    waveformPacket.data = NULL;
    waveformPacket.size = 0;
    do {
        waveformDecodePacket();
        count +=  waveformFrame->nb_samples;
    } while (waveformGotFrame);
    
    if (waveform->getSize() < WAVEFORM_SIZE)
        waveform->add(RMS(true));
    
    //std::cout << "Waveform calculation completed" << std::endl;
    
    window->drawWaveform();
    
    //std::cout << "exact duration:" << std::to_string((float)totalSamples/44100.) << ";" << totalSamples << std::endl;
    
    return true;
}


int AudioFile::waveformDecodePacket()
{
    int ret = 0;
    int decoded = waveformPacket.size;
    waveformGotFrame = 0;
    
    /* decode audio frame */
    ret = avcodec_decode_audio4(waveformCodecContext, waveformFrame, &waveformGotFrame, &waveformPacket);
    
    if (ret < 0) {
        fprintf(stderr, "Waveform: error decoding audio frame (%s)\n", av_err2str(ret));
        return ret;
    }
    
    decoded = FFMIN(ret, waveformPacket.size);
    
    if (waveformGotFrame) {
        
        float sample;
        
        for (int i = 0; i < waveformFrame->nb_samples; ++i) {
            
            ++totalSamples;
            
            if(sampleFormat == AV_SAMPLE_FMT_S16P) {
                sample = (short) (waveformFrame->extended_data[0][2 * i] | waveformFrame->extended_data[0][2 * i + 1] << 8) / SHORT_MAX;
                
                if (stereo) {
                    sample += (short) (waveformFrame->extended_data[1][2 * i] | waveformFrame->extended_data[1][2 * i + 1] << 8) / SHORT_MAX;
                    sample /= 2;
                }
                
                
                waveformBuffer[wfWritePos] = sample;
                completed++;
                
                if(++wfWritePos == 3*waveformBufferSize)
                    wfWritePos = 0;
                
            }
        }
        
    }
    
    if (wfWritePos - wfReadPos >= samplesPerChunk || wfReadPos - wfWritePos <= 2*samplesPerChunk)
        waveform->add(RMS(false));
    
    return decoded;
}

float AudioFile::RMS(bool last)
{
    float value = 0;
    int size = 0;
    
    if (last == true) {
        while (wfReadPos != wfWritePos) {
            value += waveformBuffer[wfReadPos]*waveformBuffer[wfReadPos];
            
            if (++wfReadPos == 3*waveformBufferSize)
                wfReadPos = 0;
            
            ++size;
        }
    }
    else {
        for (unsigned int i = 0; i < samplesPerChunk; ++i) {
            
            value += waveformBuffer[wfReadPos]*waveformBuffer[wfReadPos];
            
            if (++wfReadPos == 3*waveformBufferSize)
                wfReadPos = 0;
            
            ++size;
        }
    }
    
    value = sqrt((float) value / size);
    
    
    return value;
}

void AudioFile::jumpTo(int16_t time)
{
    
    if (av_seek_frame(formatContext, audioStreamIndex, time, AVSEEK_FLAG_BACKWARD) < 0) {
        std::cerr << "ffmpeg: Could not seek frame\n";
    }
}

const std::vector<float> &AudioFile::getWaveform(int size) const
{
    waveform->resize(size);
    return waveform->getResizedWaveform();
}

