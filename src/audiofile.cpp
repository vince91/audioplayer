//
//  audiofile.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#include "audiofile.h"


AudioFile::AudioFile(std::string _filename) : filename(_filename)
{
    av_register_all();
    init();
}

bool AudioFile::init()
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
    av_dump_format(formatContext, 0, filename.c_str(), 0);
    
    
    frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Could not allocate frame\n";
        return false;
    }
    
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
    
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
