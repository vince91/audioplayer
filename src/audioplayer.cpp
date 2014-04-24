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

AudioPlayer::AudioPlayer()
{
}

void AudioPlayer::play()
{
    if(playlist.size() > 0)
        AudioFile audio(playlist.front());
}

void AudioPlayer::addToPlaylist(std::string filename)
{
    std::cerr << "Adding " << filename << " to playlist\n";
    AudioPlayer::playlist.push_back(filename);
}


