//
//  main.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#include <iostream>
#include "audioplayer.h"

int main(int argc, const char * argv[])
{
    AudioPlayer player;
    
    player.addToPlaylist("/Users/vincent/Music/AVRIL HOUSE/DJ Assassin - A Face In The Crowd (Intellidread Mix).mp3");
    player.play();
    
    return 0;
}

