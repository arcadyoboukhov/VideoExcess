#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>

#include <QSlider>

#include <QLabel>

#include <QPushButton>

#include <QMediaPlayer>

#include <QVideoWidget>

#include <QStringList> // Required for QStringList

#include <QMap> // Required for QMap

#include <QPair> // Required for QPair

#include "mainwindow.h"

#include <QSoundEffect>

#include <QTimer>

#include <QFileDialog>

#include <QDebug>

#include <iostream>

#include <fstream>

#include <QDir>

#include <filesystem>

#include <QMediaPlayer>

#include <QAudioOutput>

#include "../include/nlohmann/json.hpp"

#include <QEventLoop>

#include <QUrl>

#include <QString>

#include <QMediaPlayer>

#include <QMediaMetaData>

#include <QFileInfo>

#include <QCoreApplication>


// Forward declaration of the Filex and Settings classes
class Filex;
class Settings;

namespace Ui {
class Player;
}

class Player: public QWidget {
    Q_OBJECT

public:
    explicit Player(MainWindow * mainWindow, QWidget * parent = nullptr);
    ~Player();

    void setVariable(int num);
    void playMedia(const QString & mediaPath); // Function to play media

public slots:
    void onGoBackButtonClicked();
    void onFilexButtonClicked(); // New slot for button_2
    void onPlayButtonClicked();
    int getMode() const;
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state); // Declare the slot
    void onMediaError(QMediaPlayer::Error error);
    void setMediaPosition(int position);
    void updateSliderPosition();
    void setVolume(int volume);

private:
    void playMediaAt(int index); // Declare the playMediaAt function
    bool sliderBeingDragged = false;
    Ui::Player * ui;
    MainWindow * mainWindow;
    static int mode;
    Filex * filexWindow; // Member variable for Filex window
    Settings * settingsWindow; // Member variable for Settings window
    QSoundEffect * soundEffect; // New member variable for sound effect
    QTimer * timer;


    // Media playback member variables
    QMediaPlayer * mediaPlayer; // For playback
    QAudioOutput * audioOutput; // Add this line
    QVideoWidget * videoWidget; // For displaying video
    bool isPlaying; // Flag indicating playback state
    qint64 currentPosition = 0; // Initialize currentPosition to 0
    void readAndDisplayJson(const QString & mediaPath);
    void getconfig();
    // Add draggingSlider and timer as member variables
    bool draggingSlider = false; // Initialize the draggingSlider variable
    bool mp3mode = false;
    float playback;
    bool savePositionToJson(const QString& mediaPath);
    void initConnections(); // Method for connecting signals and slots, as an example
    QString mediaPath;
    static int Dplayback;
    QList<QString> playlist;   // Assuming this holds the media paths
    int currentPlaylistIndex;  // Declare currentPlaylistIndex here

public:
    void setPlayback(int playbackValue);
    void resetMediaPlayer();
};

#endif // PLAYER_H
