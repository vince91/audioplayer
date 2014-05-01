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

class MainWindow;

class AudioPlayer
{
public:
    AudioPlayer(MainWindow *parent);
    void addToPlaylist(std::string);
    
    bool loadAndPlay(std::string);
    void pause();
    void stop(bool);
    bool isPlaying() const { return playing; }
    bool isPaused() const { return paused; }
    
private:
    MainWindow *parent;
    
    std::thread *bufferThread = nullptr;
    std::list<std::string> playlist;
    
    bool playing = false;
    bool paused = false;
    
    PaStream *stream;
    
    AudioFile *audio = nullptr;
    
    static int patestCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData );

    
};

typedef struct
{
    const float *firstChannel;
    const float *secondChannel;
    int *readPos;
    const int *lastIndex;
    AudioPlayer *player;
}
paData;


#endif /* defined(__audioplayer__audioplayer__) */
