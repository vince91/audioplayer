//
//  audioplayer.h
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#ifndef __audioplayer__audioplayer__
#define __audioplayer__audioplayer__

#include <iostream>
#include <list>
#include <thread>
#include <portaudio.h>
#include "audiofile.h"

class AudioFile;

class AudioPlayer
{
public:
    AudioPlayer();
    void addToPlaylist(std::string);
    
    bool loadAndPlay(std::string);
    void pause();
    void stop();
    
private:
    std::thread *bufferThread = nullptr;
    std::list<std::string> playlist;
    bool paused = false;
    
    PaStream *stream;
    
    AudioFile *audio;
    
    static int patestCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData );

    
};


#endif /* defined(__audioplayer__audioplayer__) */
