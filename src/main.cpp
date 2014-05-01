//
//  main.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 24/04/2014.
//  Copyright (c) 2014 Vincent Timofti. All rights reserved.
//

#include <iostream>
#include <QApplication>
#include "audioplayer.h"
#include "mainwindow.h"

int main(int argc, char * argv[])
{
    /*
    AudioPlayer player;
    
    //player.loadAndPlay("/Users/vincent/Music/AVRIL HOUSE/Mind Against - Atlant (Original Mix) [exclusive-music-dj.com].mp3");
    player.loadAndPlay("/Users/vincent/Music/test.mp3");
    //*/
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
    
    return 0;
}

