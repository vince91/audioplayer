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
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSlider>
#include "audioplayer.h"

class AudioPlayer;
class Waveform;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void updateButton();
    void updateMetadata(std::string, std::string, std::string, std::string, std::string, std::string);
    void drawWaveform();
    void clearWaveform();
    void updateTime(float);
    
private:
    QPushButton *playPauseButton;
    QPushButton *stopButton;
    QLabel *albumCover;
    QLabel *artistTitle;
    QLabel *albumYear;
    QLabel *duration;
    QLabel *genre;
    QLabel *elapsedTime;
    QGraphicsScene *waveformScene;
    QGraphicsView *waveformView;
    QSlider *slider;
    AudioPlayer player;
    
    static const int waveformHeight = 150;
    static const int margin = 10;
    static const int offset = 30;
    
private slots:
    void playPause();
    void stop();
    void sliderUpdate(int);
    void updateGUI();

protected:
    void resizeEvent(QResizeEvent* event);
    void closeEvent(QCloseEvent *event);
    
signals:
    void mainThreadSignal();

    
};


#endif /* defined(__audioplayer__mainwindow__) */
