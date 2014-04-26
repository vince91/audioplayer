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
    
    //player.loadAndPlay("/Users/vincent/Music/AVRIL HOUSE/Mind Against - Atlant (Original Mix) [exclusive-music-dj.com].mp3");
    player.loadAndPlay("/Users/vincent/Music/test.mp3");
    
    return 0;
}

