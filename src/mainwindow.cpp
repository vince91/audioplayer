//
//  mainwindow.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 01/05/2014.
//
//

#include "mainwindow.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), player(this)
{
    setWindowTitle("Audio Player");
    
    QWidget *centralWidget = new QWidget;
    
    QHBoxLayout *commandLayout = new QHBoxLayout;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    
    playPauseButton = new QPushButton("Play");
    stopButton = new QPushButton("Stop");
    
    connect(playPauseButton, SIGNAL(clicked()), this, SLOT(playPause()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    
    commandLayout->addWidget(playPauseButton);
    commandLayout->addWidget(stopButton);
    commandLayout->addStretch();
    
    mainLayout->addLayout(commandLayout);
    
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::playPause()
{
    std::cout << "play" << std::endl;
    
    if (player.isPlaying())
        player.pause();
    else
        player.loadAndPlay("/Users/vincent/Music/AVRIL HOUSE/Alejandro Mosso - Nightwalker (LoSoul Remix) www.what-dj-plays.com.mp3");
    
}

void MainWindow::stop()
{
    playPauseButton->setText("Play");
    player.stop(false);
}

void MainWindow::updateButton()
{
    if (player.isPlaying()) {
        if (player.isPaused()) {
            playPauseButton->setText("Resume");
        }
        else {
            playPauseButton->setText("Pause");
        }
    }
    else {
        playPauseButton->setText("Play");
    }
    
}
