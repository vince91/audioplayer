//
//  audioplayer.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#include "audioplayer.h"
#include "audiofile.h"
#include "mainwindow.h"
#include "waveform.h"
#include <portaudio.h>
#include <thread>
#include <QDir>

static paData playerData;

AudioPlayer::AudioPlayer(MainWindow *_window) : window(_window)
{
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
        return;
    }
    
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 256, patestCallback, &playerData );
    
    playerData.player = this;
}

bool AudioPlayer::play(AudioFile *_audio)
{
    audio = _audio;
    
    if (audio != nullptr) {
        
        if (!audio->initialize()) {
            std::cerr << "Could not load " << audio->getFilename() << std::endl;
            return false;
        }
        
        playing = true;

        window->updateMetadata(audio->title, audio->artist, audio->album, audio->year, audio->genre, audio->duration);
        
        playerData.firstChannel = audio->firstChannel;
        playerData.secondChannel = audio->secondChannel;
        playerData.readPos = &audio->readPos;
        playerData.lastIndex = &audio->lastIndex;
        playerData.playedSamples = &audio->playedSamples;
        playerData.seekRequested = &audio->seekRequested;
        playerData.newReadPos = &audio->newReadPos;
        
        Pa_StopStream(stream);
        PaError err = Pa_StartStream(stream);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
            return false;
        }
        
        bufferThread = new std::thread(&AudioFile::threadFillBuffer, audio);
        
        emit window->updateInterface();
        
        return true;
    }
    return false;
}

void AudioPlayer::pause()
{
    if (paused) {
        paused = false;
        PaError err = Pa_StartStream(stream);
        if (err != paNoError)
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }
    else {
        Pa_AbortStream(stream);
        paused = true;
    }
    
    emit window->updateInterface();
}

void AudioPlayer::stop(bool callback) {
    
    playing = false; paused = false;
    
    emit window->updateInterface();
    
    if (audio == nullptr)
        return;
    
    if (!callback)
        Pa_StopStream(stream);
    
    audio->stopThread();
    
    if (bufferThread != nullptr) {
        if (bufferThread->joinable()) {
            bufferThread->join();
            delete bufferThread;
        }
    }
    
    audio = nullptr;
}


int AudioPlayer::patestCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    /* Cast data passed through stream to our structure. */
    paData *data = (paData*) userData;
    int *readPos = data->readPos;
    float *out = (float*)outputBuffer;
    (void) inputBuffer; (void) timeInfo; (void) statusFlags;
    
    for (unsigned int i = 0; i < framesPerBuffer; ++i) {
        
        if (*readPos == *(data->lastIndex)) {
            //std::cout << "paComplete\n" << *data->playedSamples << std::endl;;
            *out++ = 0.;
            *out++ = 0.;
            data->player->stop(true);
            return paComplete;
        }
        else {
            
            *out++ = data->firstChannel[*readPos]; /* left channel */
            *out++ = data->secondChannel[*readPos]; /* right channel */
            
            ++(*data->playedSamples);
            
            if (++(*readPos) == 3*BUFFER_SIZE)
                *readPos = 0;
        }
        
    }
    
    return paContinue;
}

const std::vector<float> & AudioPlayer::getWaveform(int lenght) const
{
    audio->waveform->resize(lenght);
    return audio->waveform->getResizedWaveform();
}

std::string AudioPlayer::getDurationString() const
{
    return audio->duration;
}

float AudioPlayer::getDuration() const
{
    return (float)audio->totalSamples/audio->codecContext->sample_rate;
}

void AudioPlayer::jumpTo(int value)
{
    if (playing) {
        
        //audio->seekRequested = true;
        
        int64_t time = value/1000. * getDuration() * audio->audioStream->time_base.den / audio->audioStream->time_base.num;
        
        audio->seekTime = time;
        
        Pa_AbortStream(stream);
        
        audio->playedSamples = value/1000. * audio->totalSamples;
        av_seek_frame(audio->formatContext, audio->audioStreamIndex, time, AVSEEK_FLAG_BACKWARD);
        
        audio->readPos = audio->writePos + 1;
        audio->fillBuffer();
        
        //Pa_Sleep(1000);
        Pa_StartStream(stream);
        
    }
}




