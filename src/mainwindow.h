//
//  mainwindow.h
//  audioplayer
//
//  Created by Vincent Timofti on 01/05/2014.
//
//

#ifndef __audioplayer__mainwindow__
#define __audioplayer__mainwindow__

#include <iostream>
#include <QMainWindow>
#include <QPushButton>
#include "audioplayer.h"

class AudioPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateButton();
    
private:
    QPushButton *playPauseButton;
    QPushButton *stopButton;
    AudioPlayer player;
    
private slots:
    void playPause();
    void stop();
    
};


#endif /* defined(__audioplayer__mainwindow__) */
