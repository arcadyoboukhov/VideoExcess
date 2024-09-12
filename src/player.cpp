#include "player.h"

#include "ui_player.h"

#include <iostream>

#include "filex.h"

#include <QMediaPlayer>

#include <QVideoWidget>

#include <QSoundEffect>

#include <QAudioOutput>

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




int Player::mode = 0; // Static member definition outside the class
int Player::Dplayback = 1;
int currentPlaylistIndex = 0; // Start from the first track

Player::Player(MainWindow * mainWindow, QWidget * parent): QWidget(parent), ui(new Ui::Player), mainWindow(mainWindow), filexWindow(nullptr), settingsWindow(nullptr), soundEffect(new QSoundEffect(this)) {
    ui -> setupUi(this);
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this); // Initialize QAudioOutput
    videoWidget = new QVideoWidget(this);


    timer = new QTimer(this);
    connect(timer, & QTimer::timeout, this, & Player::updateSliderPosition);
    timer -> start(1000); // Update every second

    // Set the layout to cover the entire group box
    QVBoxLayout * layout = new QVBoxLayout(ui -> groupBox); // Create a vertical layout
    layout -> addWidget(videoWidget); // Add the video widget to that layout
    ui -> groupBox -> setLayout(layout); // Set the layout for the group box

    mediaPlayer -> setVideoOutput(videoWidget);
    mediaPlayer -> setAudioOutput(audioOutput); // Set the audio output immediately

    connect(ui -> pushButton, & QPushButton::clicked, this, & Player::onGoBackButtonClicked);
    connect(ui -> pushButton_2, & QPushButton::clicked, this, & Player::onFilexButtonClicked);
    connect(ui -> pushButton_3, & QPushButton::clicked, this, & Player::onPlayButtonClicked);
    connect(ui -> horizontalSlider, & QSlider::valueChanged, this, & Player::setMediaPosition);
    connect(ui -> horizontalSlider_2, QOverload < int > ::of( & QSlider::valueChanged), this, & Player::setVolume);

    // Connect the signal for mediaPlayer to handle when one video ends and the next one starts
    connect(mediaPlayer, & QMediaPlayer::playbackStateChanged, this, & Player::onPlaybackStateChanged);
    connect(mediaPlayer, & QMediaPlayer::errorOccurred, this, & Player::onMediaError); // Connect error signal
    connect(mediaPlayer, & QMediaPlayer::positionChanged, this, & Player::updateSliderPosition);
    connect(mediaPlayer, & QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui -> horizontalSlider -> setRange(0, static_cast < int > (duration));

        // Simplified connection for slider dragging handling
        connect(ui -> horizontalSlider, & QSlider::sliderPressed, this, [this]() {
            draggingSlider = true; // User started dragging
        });

        connect(ui -> horizontalSlider, & QSlider::sliderReleased, this, [this]() {
            draggingSlider = false; // User finished dragging
            mediaPlayer -> setPosition(ui -> horizontalSlider -> value() * 1000); // Set position when released, multiply by 1000 to convert seconds to milliseconds
        });

    });
}
void Player::setVolume(int volume) {
    qreal newVolume = std::clamp(static_cast < qreal > (volume) / 100.0, 0.0, 1.0);
    audioOutput -> setVolume(newVolume);
    std::cout << "Volume set to: " << newVolume << std::endl;
    ui -> label_2 -> setText(QString::number(static_cast < int > (newVolume * 100)) + "%");
}

void Player::setMediaPosition(int position) {
    if (draggingSlider) {
        mediaPlayer -> setPosition(position);
        if (mediaPlayer -> playbackState() == QMediaPlayer::PlayingState) {
            mediaPlayer -> play(); // Ensure audio is playing when changing position
        }
    }
}

void Player::updateSliderPosition() {
    // Update the slider position only when not dragging
    if (!draggingSlider) {
        qint64 position = mediaPlayer -> position();
        ui -> horizontalSlider -> setValue(static_cast < int > (position / 1000)); // Convert ms to seconds
        ui -> label -> setText(QString::number(position / 100000.0, 'f', 2) + " minutes"); // Current position in minutes
    }
}

void Player::readAndDisplayJson(const QString & mediaPath) {
    // Derive the JSON file path from the media path
    QFileInfo mediaInfo(mediaPath);
    QString jsonFileName = mediaInfo.baseName() + ".json"; // Assuming the JSON file has the same name as the media file
    QString jsonFilePath = mediaInfo.absolutePath() + "/" + jsonFileName; // Create the full path for the JSON file

    // Attempt to open the JSON file
    std::ifstream jsonFile(jsonFilePath.toStdString());
    if (!jsonFile.is_open()) {
        std::cerr << "Could not open JSON file: " << jsonFilePath.toStdString() << std::endl;
        return;
    }

    try {
        // Parse the JSON file
        nlohmann::json jsonData;
        jsonFile >> jsonData;

        // Check if 'Time' key exists and read it
        if (jsonData.contains("Time")) {
            int start = jsonData["Time"]; // Use at your own risk
            currentPosition = start;
            std::cout << "Starting position set to: " << start << std::endl;
        } else {
            std::cerr << "Key 'Time' not found in JSON file." << std::endl;
        }

        // Print the data to the console, you can modify this to display it in the GUI
        std::cout << "JSON data for " << mediaPath.toStdString() << ":\n";
        std::cout << jsonData.dump(4); // Pretty print with 4 spaces

    } catch (const std::exception & e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl; // Catch and output any exceptions
    }

    jsonFile.close();
}

void Player::onFilexButtonClicked() {
    this -> hide(); // Hide the Player window

    if (settingsWindow) {
        settingsWindow -> hide(); // Ensure settingsWindow is valid
    }

    // Cleanup previous Filex window if it exists
    if (filexWindow) {
        delete filexWindow;
        filexWindow = nullptr; // Avoid dangling pointer
    }

    // Create and show the new Filex window
    filexWindow = new Filex(mainWindow, this, settingsWindow);
    filexWindow -> show();
}


// Modified onPlayButtonClicked()
void Player::onPlayButtonClicked() {
    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        savePositionToJson(playlist.at(currentPlaylistIndex)); // Save position of the currently playing track
        currentPosition = mediaPlayer->position();
        mediaPlayer->pause();
        isPlaying = false;
        ui->pushButton_3->setText("Play");


        if (this->playlist.isEmpty()) {
            return; // If there's nothing in the playlist, do nothing
        }

    } else {
        if (filexWindow) {
            QMap<QString, QPair<QString, QString>> mediaMap = filexWindow->getMediaMap();
            if (!mediaMap.isEmpty()) {
                playlist.clear(); // Clear existing playlist before populating with new one
                for (auto it = mediaMap.constBegin(); it != mediaMap.constEnd(); ++it) {
                    QString mediaPath = it.value().first; // Get media file path
                    playlist.append(mediaPath); // Add to playlist
                    readAndDisplayJson(mediaPath); // Read JSON for each media file
                }

                // Check for folder mode
                if (Player::mode == 1) { // Assuming mode 1 is folder mode
                    QDir directory; // Initialize to the relevant directory
                    directory.setPath("/path/to/your/folder"); // Example path, replace it with actual folder
                    for (const QString &file : directory.entryList(QDir::Files)) {
                        this -> playlist.append(directory.filePath(file)); // Add each file to the playlist
                    }
                }

                playMediaAt(currentPlaylistIndex); // Start playing the first media
                mediaPlayer->setPosition(currentPosition); // Resume from the stored position

                int slider3Value = ui->horizontalSlider_3->value();
                if (slider3Value == 2) {
                    mediaPlayer->setPlaybackRate(2.0); // Set playback speed to 2x
                } else if (slider3Value == 0) {
                    mediaPlayer->setPlaybackRate(0.5); // Set playback speed to 0.5x
                } else {
                    mediaPlayer->setPlaybackRate(1.0); // Set playback speed to 1x
                }

                mediaPlayer->play(); // Start playback
                isPlaying = true; // Update the playing state
                ui->pushButton_3->setText("Pause"); // Update button text to Pause
            } else {
                std::cerr << "Media map is empty." << std::endl;
            }
        } else {
            std::cerr << "filexWindow is not initialized." << std::endl;
        }
    }
}

// Modified playMediaAt()
void Player::playMediaAt(int index) {
    if (index >= 0 && index < playlist.size()) {
        // Disconnect any previous playbackStateChanged connections
        disconnect(mediaPlayer, &QMediaPlayer::playbackStateChanged, nullptr, nullptr);
        QString mediaPath = playlist.at(index);
        std::cout << "Attempting to play media: " << mediaPath.toStdString() << std::endl;

        mediaPlayer->setSource(QUrl::fromLocalFile(mediaPath));

        // Ensuring the proper handling of duration change and update
        connect(mediaPlayer, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
            ui->horizontalSlider->setRange(0, static_cast<int>(duration / 1000)); // Duration in seconds
            ui->label_3->setText(QString::number(duration / 100000.0, 'f', 2) + " minutes");
        });

        // Attempt to read the last position from JSON file
        readAndDisplayJson(mediaPath); // This will set currentPosition if the JSON key exists

        mediaPlayer->setPosition(currentPosition); // Resume from the loaded position


        // Show/hide video widget based on the media type
        if (mediaPath.endsWith(".mp3", Qt::CaseInsensitive)) {
            videoWidget->hide(); // Hide video widget for audio playback
        } else {
            videoWidget->show(); // Show video widget for video playback
        }

        mediaPlayer->play(); // Start playback
        isPlaying = true; // Update playing state
        setVolume(ui->horizontalSlider_2->value());

        // Handle playback state change
        connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, this, [this, index](QMediaPlayer::PlaybackState state) {
            if (state == QMediaPlayer::StoppedState) {
                if (index + 1 < playlist.size()) {
                    playMediaAt(index + 1); // Play next track if available
                    currentPlaylistIndex++;
                } else {
                    resetMediaPlayer(); // Reset if no more tracks
                }
            }
        });
    }
}



void Player::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
    isPlaying = (state == QMediaPlayer::PlayingState);
    if (!isPlaying) {
        if (mediaPlayer->playbackState() == QMediaPlayer::StoppedState) {
            if (currentPlaylistIndex + 1 < playlist.size()) {
                playMediaAt(currentPlaylistIndex + 1); // continue to next
            } else {
                resetMediaPlayer(); // Reset when playlist ends
            }
        }
    }
    if (isPlaying) {
        ui -> pushButton_3 -> setText("Pause"); // Change to "Pause" when playing
    } else {
        ui -> pushButton_3 -> setText("Play"); // Change to "Play" when paused
    }
}



// New method to reset the media player state
void Player::resetMediaPlayer() {
    mediaPlayer -> stop(); // Stop playback
    ui -> horizontalSlider -> setValue(0); // Reset slider position to 0
    ui -> label -> setText("0 minutes"); // Reset the current position label
    ui -> pushButton_3 -> setText("Play"); // Change button text back to "Play"
    playlist.clear(); // Clear the playlist
    filexWindow->resetMediaMap();
}

void Player::setVariable(int num) {
    mode = num;
    std::cout << mode << std::endl;
}
// On back button clicked
void Player::onGoBackButtonClicked() {
    mediaPlayer -> stop(); // Stop the media player when going back
    // Ensure there's no crash if no media was selected
    if (filexWindow) {
        // Check if the media map is valid
        if (filexWindow->getMediaMap().isEmpty()) {
            std::cerr << "No media selected. Returning to main window." << std::endl;
        } else {
            // Perform any clean-up if necessary
            filexWindow->resetMediaMap(); // Reset media map if needed
        }
    } else {
        std::cerr << "filexWindow is not initialized. Returning to main window." << std::endl;
    }
    this -> hide(); // Hide the Player window
    mainWindow -> show(); // Show the main window
}

void Player::onMediaError(QMediaPlayer::Error error) {
    std::cerr << "Media error occurred: " << error << std::endl;
}


int Player::getMode() const {
    return mode; // Return the current value of mode
}
bool Player::savePositionToJson(const QString& mediaPath) {
    currentPosition = mediaPlayer->position();
    QFileInfo mediaInfo(mediaPath);
    QString jsonFilePath = mediaInfo.absolutePath() + "/" + mediaInfo.baseName() + ".json";

    nlohmann::json jsonData;

    // Attempt to open and read the existing JSON file
    std::ifstream jsonFile(jsonFilePath.toStdString());
    if (jsonFile.is_open()) {
        try {
            // Parse the existing JSON data
            jsonFile >> jsonData;
            jsonFile.close(); // Close after reading
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            jsonFile.close();
            return false; // Exit early if there's a parsing error
        }
    }

    // Update the jsonData with current position
    jsonData["Time"] = currentPosition;

    // Open the file again (this time for writing)
    std::ofstream outFile(jsonFilePath.toStdString());
    if (outFile.is_open()) {
        outFile << jsonData.dump(4); // Save formatted JSON
        outFile.close();
        std::cout << "Position saved to JSON: " << currentPosition << std::endl;
        void readAndDisplayJson(const QString & mediaPath);
        return true; // Indicate success
    } else {
        std::cerr << "Could not open JSON file for writing: " << jsonFilePath.toStdString() << std::endl;
        void readAndDisplayJson(const QString & mediaPath);
        return false; // Indicate failure
    }
}

void Player::setPlayback(int playbackValue) {
        ui -> horizontalSlider_3 -> setValue(playbackValue);
}

Player::~Player() {
    // Only delete if they are allocated
    delete ui;
    if (mediaPlayer) {
        delete mediaPlayer;
    }
    if (videoWidget) {
        delete videoWidget;
    }
    if (filexWindow) {
        delete filexWindow;
    }
    if (settingsWindow) {
        delete settingsWindow;
    }
}
