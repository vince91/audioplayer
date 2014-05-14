//
//  mainwindow.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 01/05/2014.
//
//

#include "mainwindow.h"
#include "waveform.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QDir>
#include <QGraphicsView>
#include <QPen>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), player(this, QDir::tempPath().toStdString())
{
    setWindowTitle("Audio Player");
    
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    
    /* command area */
    QHBoxLayout *commandLayout = new QHBoxLayout;
    
    playPauseButton = new QPushButton("Play");
    stopButton = new QPushButton("Stop");
    
    QPushButton *temp = new QPushButton("Waveform");
    
    connect(playPauseButton, SIGNAL(clicked()), this, SLOT(playPause()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    //connect(temp, SIGNAL(clicked()), this, SLOT(drawWaveform()));
    
    elapsedTime = new QLabel("00:00");
    
    commandLayout->addWidget(playPauseButton);
    commandLayout->addWidget(stopButton);
    commandLayout->addWidget(elapsedTime);
    //commandLayout->addWidget(temp);
    commandLayout->addStretch();
    
    mainLayout->addLayout(commandLayout);
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    
    slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(1000);    //slider->
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderUpdate(int)));
    mainLayout->addWidget(slider);
    
    mainLayout ->addWidget(line);
    
    /* informations area */
    QHBoxLayout *informationsLayout = new QHBoxLayout;
    QVBoxLayout *labelsLayout = new QVBoxLayout;
    
    QPixmap album_cover(":/images/album_cover.png");
    albumCover = new QLabel;
    albumCover->setPixmap(album_cover.scaled(150, 150, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    artistTitle = new QLabel("[Artist] - [Title]");
    albumYear = new QLabel("Album: [Album], [Year]");
    duration = new QLabel("Duration: [duration]");
    genre = new QLabel("Genre: [Genre]");
    
    
    labelsLayout->addWidget(artistTitle);
    labelsLayout->addWidget(albumYear);
    labelsLayout->addWidget(duration);
    labelsLayout->addWidget(genre);
    labelsLayout->addStretch();
    
    informationsLayout->addWidget(albumCover);
    informationsLayout->addLayout(labelsLayout);
    informationsLayout->addStretch();
    
    mainLayout->addLayout(informationsLayout);
    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);
    
    /* waveform area */
    
    waveformScene = new QGraphicsScene();
    waveformView = new QGraphicsView(waveformScene);
    
    waveformView->setFixedHeight(waveformHeight);
    waveformView->setStyleSheet( "QGraphicsView { border-style: none; }" );
    mainLayout->addWidget(waveformView);
    /* playlist area */
    
    //mainLayout->addStretch();
    
    
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    
    this->setMinimumWidth(600);
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::playPause()
{
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

void MainWindow::updateMetadata(std::string _title, std::string _artist, std::string _album, std::string _year, std::string _genre, std::string _duration)
{
    _title = _artist + " - " + _title;
    artistTitle->setText(QString(_title.c_str()));
    
    _album = "<i>Album:</i> " + _album;
    if (!_year.empty())
        _album = _album + ", " + _year;
    
    albumYear->setText(QString(_album.c_str()));
    
    _genre = "<i>Genre:</i> " + _genre;
    genre->setText(QString(_genre.c_str()));
    
    _duration = "<i>Duration:</i> " + _duration;
    duration->setText(QString(_duration.c_str()));
    
    /* album cover */
    std::string coverFilename = QDir::tempPath().toStdString() + "/audio_player_cover";
    if (QFile::exists(QString(coverFilename.c_str()))) {
        QPixmap albumCoverPix(coverFilename.c_str());
        albumCover->setPixmap(albumCoverPix.scaled(150, 150, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    
    
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //std::cout << waveformView->width() << std::endl;
    //drawWaveform();
    
    (void)event;
    
    
    
}

void MainWindow::drawWaveform()
{
    int newSize = waveformView->width();//- margin;
    int sceneHeight = waveformHeight - margin;
    
    
    std::vector<float> waveform = player.getWaveform(newSize);
    
    
    if (waveform.size() > 0) {
                
        QPen topPen;
        QPen bottonPen;
    
        waveformScene->setSceneRect(0, -sceneHeight/2., newSize, sceneHeight);
        
        topPen.setColor(QColor(191, 11, 50));
        bottonPen.setColor(QColor(191, 11, 50, 50));

        float max = 0;
        float current;
        for (unsigned int i = 0; i < waveform.size(); ++i) {
            if ((current = waveform[i]) > max)
                max = current;
        }
        
        
        qreal x, h;
        
        for (int i = 0; i < newSize; ++i) {
            x = i+0.5;
            h = waveform[i]/max * (sceneHeight/2. + offset);
            
            waveformScene->addLine(x, -h + offset, x, offset, topPen);
            waveformScene->addLine(x, offset, x, offset + 0.4*h, bottonPen);
            
        }
    }
    
    //Waveform::test();
}

void MainWindow::sliderUpdate(int value)
{
    std::cout << value << std::endl;
}

void MainWindow::clearWaveform()
{
    waveformScene->clear();
}

void MainWindow::updateTime(float time)
{
    int minuts = time/60;
    int seconds = time - minuts*60;
    std::string s = std::to_string(minuts) + ":" + std::to_string(seconds);
    
    elapsedTime->setText(s.c_str());
}


