//
//  audiofile.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#include <portaudio.h>
#include "audiofile.h"
#define SHORT_MAX 32768.


AudioFile::AudioFile(std::string _filename) : filename(_filename)
{
    av_register_all();
}

AudioFile::~AudioFile()
{
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);
    av_frame_free(&frame);
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
    
    /* open the apropriate codec */
    if (!openCodecContext()) {
        std::cerr << "Could not open MP3 codec\n" << std::endl;
        return false;
    }
    
    /* dump file info to stderr */
    //av_dump_format(formatContext, 0, filename.c_str(), 0);
    
    sampleFormat = formatContext->streams[streamIndex]->codec->sample_fmt;
    
    if (codecContext->channels == 2)
        stereo = true;
    
    frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Could not allocate frame\n";
        return false;
    }
    
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
    
    fillBuffer();
    
    return true;
}

bool AudioFile::openCodecContext()
{
    AVCodec *codec = NULL;
    
    streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    
    if (streamIndex < 0) {
        std::cerr << "Could not find audio stream in input file " << filename << std::endl;
        return false;
    }
    
    stream = formatContext->streams[streamIndex];
    
    /* find decoder for the stream */
    codecContext = stream->codec;
    codec = avcodec_find_decoder(codecContext->codec_id);
    
    if (!codec) {
        std::cerr << "Failed to find audio codec\n";
        return false;
    }
    
    /* Init the decoders without reference counting */
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
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

                if(++writePos == 3*BUFFER_SIZE)
                    writePos = 0;
                
            }
            
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
    
    total += count;
    
    bool ret = (count >= BUFFER_SIZE);
    
    if (!ret)
        lastIndex = writePos;
    
    return ret; /* returns false when entire audio file has been read */
}

void AudioFile::threadFillBuffer()
{
    /* fill circular buffer so portaudio as always enough data to process */
    
    while (lastIndex == -1) {
        if (writePos > readPos) {
            if ((writePos - readPos - 1) < BUFFER_SIZE)
                fillBuffer();
        }
        else {
            if ((writePos + 3*BUFFER_SIZE - readPos) < BUFFER_SIZE)
                fillBuffer();
        }
        
        Pa_Sleep(10);
    }
    
}