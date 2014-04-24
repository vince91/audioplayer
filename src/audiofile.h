//
//  audiofile.h
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#ifndef __audioplayer__audiofile__
#define __audioplayer__audiofile__

#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
}

class AudioFile
{
public:
    AudioFile(std::string _filename);
    
private:
    std::string filename;
    bool init();
    bool openCodecContext();
    
    AVFormatContext *formatContext = NULL;
    AVCodecContext *codecContext = NULL;
    AVPacket packet;
    AVFrame *frame = NULL;
    AVStream *stream = NULL;
    int streamIndex = -1;
    
    
    
    
};

#endif /* defined(__audioplayer__audiofile__) */
