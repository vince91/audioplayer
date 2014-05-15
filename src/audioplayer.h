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
#include <vector>
#include <portaudio.h>
#include "audiofile.h"

class MainWindow;

class AudioPlayer
{
public:
    AudioPlayer(MainWindow *, std::string);
    void addToPlaylist(std::string);
    
    bool loadAndPlay(std::string);
    void pause();
    void stop(bool);
    void jumpTo(float);
    bool isPlaying() const { return playing; }
    bool isPaused() const { return paused; }
    const std::vector<float> & getWaveform(int) const;
    std::string getDurationString() const;
    float getDuration() const;
    
private:
    MainWindow *window;
    
    std::thread *bufferThread = nullptr;
    std::list<std::string> playlist;
    std::string tempFolder;
    
    bool playing = false, paused = false;
    
    PaStream *stream;
    
    AudioFile *audio = nullptr;
    
    static int patestCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData );

    
};

typedef struct
{
    int *readPos;
    uint32_t *playedSamples;
    const float *firstChannel;
    const float *secondChannel;
    const int *lastIndex;
    AudioPlayer *player;
}
paData;


#endif /* defined(__audioplayer__audioplayer__) */
