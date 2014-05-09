//
//  audiofile.h
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#ifndef __audioplayer__audiofile__
#define __audioplayer__audiofile__

#define BUFFER_SIZE 10000

#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
}

class AudioPlayer;

class AudioFile
{
    friend class AudioPlayer;
    
public:
    AudioFile(std::string);
    ~AudioFile();
    
    const float * getFirstChannel() const { return firstChannel; }
    const float * getSecondChannel() const { return secondChannel; }
    int * getReadPosition() { return &readPos; }
    const int * getLastIndex() const { return &lastIndex; }
    
    bool initialize();
    void stopThread() { lastIndex = 0; }
    
    int total = 0;
    void threadFillBuffer();
    
 
    
private:
    std::string filename;
    AVSampleFormat sampleFormat;
    
    bool openAudioCodecContext();
    bool openVideoCodecContext();
    
    int decodePacket();
    bool fillBuffer();
    
    bool stereo = false;

    int writePos = 0;
    int readPos = 0;
    int lastIndex = -1;
    
    float firstChannel[3*BUFFER_SIZE];
    float secondChannel[3*BUFFER_SIZE];

    AVFormatContext *formatContext = NULL;
    AVCodecContext *audioCodecContext = NULL, *videoCodecContext = NULL;
    AVPacket packet;
    AVFrame *frame = NULL;
    AVStream *audioStream = NULL, *videoStream = NULL;
    int audioStreamIndex = -1, videoStreamIndex = -1, gotFrame;
    
    std::string artist, title, album, genre, year, duration;
    
};


#endif /* defined(__audioplayer__audiofile__) */
