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

class MainWindow;
class AudioPlayer;
class Waveform;

class AudioFile
{
    friend class AudioPlayer;
    
public:
    AudioFile(MainWindow*, std::string, std::string);
    ~AudioFile();
    
    bool initialize();
    void stopThread() { lastIndex = -2; }
    
    void threadFillBuffer();

    
private:
    MainWindow* window;
    std::string filename;
    AVSampleFormat sampleFormat;
    
    bool openCodecContext(AVFormatContext*, int &streamIndex, AVStream * &stream, AVCodecContext * &cdcContext);
    int decodePacket();
    int waveformDecodePacket();
    bool fillBuffer();
    
    void retrieveMetadata();
    void saveAlbumCover();
    bool createWaveform();
    
    void jumpTo(int16_t);
    
    bool stereo = false;

    int writePos = 0, readPos = 0, lastIndex = -1;
    float firstChannel[3*BUFFER_SIZE], secondChannel[3*BUFFER_SIZE];
    uint32_t playedSamples = 0;

    AVFormatContext *formatContext = NULL;
    AVCodecContext *codecContext = NULL;
    AVPacket packet;
    AVFrame *frame = NULL;
    AVStream *audioStream = NULL, *videoStream = NULL;
    int audioStreamIndex = -1, videoStreamIndex = -1, gotFrame;
    
    /* waveframe related variables */
    AVFormatContext *waveformFormatContext = NULL;
    AVCodecContext *waveformCodecContext = NULL;
    AVFrame *waveformFrame = NULL;
    AVPacket waveformPacket;
    int waveformGotFrame;
    
    std::string artist, title, album, genre, year, duration, tempFolder;
    
    Waveform *waveform;
    float *waveformBuffer; unsigned int wfReadPos = 0, wfWritePos = 0, samplesPerChunk, waveformBufferSize;
    float RMS(bool);
    std::thread *waveformThread = nullptr;
    
    long int completed = 0;
    uint32_t totalSamples = 0;
    
    bool seekRequested = false;
    int64_t seekTime;
    int newReadPos = 0;
    
};


#endif /* defined(__audioplayer__audiofile__) */
