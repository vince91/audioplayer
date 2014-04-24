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

class AudioPlayer
{
public:
    AudioPlayer();
    void addToPlaylist(std::string);
    
    void play();
    void pause();
    void stop();
    
private:
    std::list<std::string> playlist;
    bool paused = false;

    
};


#endif /* defined(__audioplayer__audioplayer__) */
