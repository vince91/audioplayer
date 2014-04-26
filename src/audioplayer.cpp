//
//  audioplayer.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#include "audioplayer.h"
#include "audiofile.h"
#include <portaudio.h>
#include <thread>

typedef struct
{
    const float *firstChannel;
    const float *secondChannel;
    int *readPos;
    const int *lastIndex;
    AudioPlayer *player;
}
paData;

static paData playerData;

AudioPlayer::AudioPlayer()
{
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
        return;
    }
    
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                               0,          /* no input channels */
                               2,          /* stereo output */
                               paFloat32,  /* 32 bit floating point output */
                               44100,
                               256,        /* frames per buffer, i.e. the number
                                            of sample frames that PortAudio will
                                            request from the callback. Many apps
                                            may want to use
                                            paFramesPerBufferUnspecified, which
                                            tells PortAudio to pick the best,
                                            possibly changing, buffer size.*/
                               patestCallback, /* this is your callback function */
                               &playerData ); /*This is a pointer that will be passed to
                                               your callback*/
    
    playerData.player = this;
}

bool AudioPlayer::loadAndPlay(std::string filename)
{
    audio = new AudioFile(filename);
    
    if (!audio->initialize()) {
        std::cerr << "Could not load " << filename << std::endl;
        return false;
    }
    
    playerData.firstChannel = audio->getFirstChannel();
    playerData.secondChannel = audio->getSecondChannel();
    playerData.readPos = audio->getReadPosition();
    playerData.lastIndex = audio->getLastIndex();
    
    
    PaError err;
    
    err = Pa_StartStream( stream );
    
    bufferThread = new std::thread(&AudioFile::threadFillBuffer, audio);
    
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
        return false;
    }
    
    
    while(1) {
        Pa_Sleep(10);
    }
    
    return true;
}

void AudioPlayer::stop() {
    
    PaError err;
    
    err = Pa_StopStream(stream);
    if(err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
    }
    
    if (bufferThread != nullptr) {
        if (bufferThread->joinable()) {
            bufferThread->join();
            delete bufferThread;
        }
    }
}

void AudioPlayer::addToPlaylist(std::string filename)
{
    std::cerr << "Adding " << filename << " to playlist\n";
    AudioPlayer::playlist.push_back(filename);
}

int AudioPlayer::patestCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    /* Cast data passed through stream to our structure. */
    paData *data = (paData*) userData;
    int *readPos = data->readPos;
    float *out = (float*)outputBuffer;
    
    for (int i = 0; i < framesPerBuffer; ++i) {
        
        if (*readPos == *(data->lastIndex)) {
            std::cout << "finished\n";
            *out++ = 0.;
            *out++ = 0.;
            data->player->stop();
        }
        else {
        *out++ = data->firstChannel[*readPos]; /* left channel */
        *out++ = data->secondChannel[*readPos]; /* right channel */
        
        if (++(*readPos) == 3*BUFFER_SIZE)
            *readPos = 0;
        }
        
    }
    
    return 0;
}


