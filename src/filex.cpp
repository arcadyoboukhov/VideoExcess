#include "filex.h"

#include "ui_filex.h"

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


using json = nlohmann::json;

int mediaDuration = 0; // Store media duration here

int skipped = 0;
double percentage = 0;

Filex::Filex(MainWindow * mainWindow, Player * playerInstance, Settings * settingsInstance, QWidget * parent): QWidget(parent), ui(new Ui::Filex), mainWindow(mainWindow), playerInstance(playerInstance), settingsInstance(settingsInstance) {
    ui -> setupUi(this);

    // Connecting UI button signals to slots
    connect(ui -> pushButton, & QPushButton::clicked, this, & Filex::onGoBackButtonClicked);
    connect(ui -> pushButton_2, & QPushButton::clicked, this, & Filex::onSelectDirectoryButtonClicked);
    int modeValue = playerInstance -> getMode();
    std::string modeString; // Declare modeString outside the if-else blocks

    if (modeValue == 1) {
        modeString = "Folder Mode"; // Assign value to modeString
    } else {
        modeString = "File Mode"; // Assign value to modeString
    }

    // Convert std::string to QString and set the text
    ui -> label_3 -> setText(QString::fromStdString(modeString));

    // Load configuration settings
    std::string configLocation = settingsInstance -> GetConfigLocation();
    std::ifstream inFile(configLocation);
    if (inFile.is_open()) {
        json loadedJson;
        inFile >> loadedJson; // Load the JSON from the file
        complete = loadedJson.value("complete", 80); // Initialize complete
        inFile.close(); // Close the file
    } else {
        std::cerr << "Failed to open " << configLocation << " for reading." << std::endl;
    }

}

void Filex::onGoBackButtonClicked() {
    // Hide settings if it's visible
    if (settingsInstance && settingsInstance -> isVisible()) {
        settingsInstance -> hide(); // Hide Settings
    }

    // Show the Player window
    playerInstance -> show(); // Show the Player window

    this -> close(); // Close the Filex window
}

// On "select directory" button clicked
void Filex::onSelectDirectoryButtonClicked() {
    playerInstance->resetMediaPlayer();

    int modeValue = playerInstance -> getMode();
    std::string configLocation = settingsInstance -> GetConfigLocation();
    std::cout << configLocation << '\n';

    // Declare mp3mode outside the if block
    bool mp3mode = false; // Default value

    // Read the JSON object from the file and print it
    std::ifstream inFile(configLocation);
    if (inFile.is_open()) {
        json loadedJson;
        inFile >> loadedJson; // Load the JSON from the file
        inFile.close(); // Close the file

        // Access data using key with existence checks
        int complete = loadedJson.value("complete", 80); // default to 80% if not found
        mp3mode = loadedJson.value("mode", false); // Use the previously declared variable
        playback = loadedJson.value("playback", 1); // default to 1.0 if not found

        // Add this block to set playback
        int playbackValue = playback; // You can adjust this value based on your application's logic.
        playerInstance->setPlayback(playbackValue);


        // Print the loaded JSON
        std::cout << "Loaded JSON settings from file: " << loadedJson.dump(4) << std::endl; // Pretty print
    } else {
        std::cerr << "Failed to open " << configLocation << " for reading." << std::endl;
    }

    if (modeValue == 1) // Just a reminder: Ensure modeValue is set correctly in Player.
    {
        QString directory = QFileDialog::getExistingDirectory(this, tr("Select Directory"), QDir::homePath());
        if (!directory.isEmpty()) {
            qDebug() << "Selected directory:" << directory;
            processMediaFiles(directory); // Call the new function
        }

    } else {
        QStringList filePaths; // Initialize filePaths
        if (mp3mode) {
            filePaths = QFileDialog::getOpenFileNames(this, tr("Select MP3 Files"), QDir::homePath(), tr("MP3 Files (*.mp3);;All Files (*)"));
        } else {
            filePaths = QFileDialog::getOpenFileNames(this, tr("Select MP4 Files"), QDir::homePath(), tr("MP4 Files (*.mp4);;All Files (*)"));
        }

        // Process the selected individual media files
        if (!filePaths.isEmpty()) {
            for (const QString & filePath: filePaths) {
                processMediaFile(filePath); // Process individual media file
            }
        }
    }
}

void Filex::processMediaFile(const QString & filePath) {
    QString fileName = QFileInfo(filePath).fileName();
    QString jsonFilePath = QFileInfo(filePath).absolutePath() + "/" + fileName.section('.', 0, 0) + ".json"; // Use absolute path to create JSON

    if (QFile::exists(jsonFilePath)) {

        jsonAlreadyCreated(jsonFilePath, filePath, fileName);
        getAllMediaNames();

    } else {
        // JSON file does not exist, create one with default values
        qint64 totalLength = getMediaDuration(filePath); // Use method to get duration.

        // Check if totalLength is valid
        if (totalLength <= 0) {
            std::cerr << "Error: Could not determine duration for file: " << filePath.toStdString() << std::endl;
            return; // Exit function early if duration is invalid
        }

        json newJson;
        newJson["Time"] = 0; // Default value
        newJson["total"] = totalLength; // Insert total length

        std::ofstream outFile(jsonFilePath.toStdString());
        if (outFile.is_open()) {
            outFile << newJson.dump(4); // Pretty print JSON
            outFile.close();
            qDebug() << "Created JSON file for:" << fileName;
            mediaMap[fileName] = qMakePair(filePath, jsonFilePath); // Add to map
            getAllMediaNames();
        } else {
            qDebug() << "Failed to create JSON file for:" << fileName;
        }
    }

    // Log the collected data
    for (auto it = mediaMap.begin(); it != mediaMap.end(); ++it) {
        qDebug() << "File:" << it.key() << " | Path:" << it.value().first << " | JSON Path:" << it.value().second;
    }
}

qint64 Filex::getMediaDuration(const QString & filePath) {
    QMediaPlayer mediaPlayer;
    QEventLoop eventLoop;

    // Connect the durationChanged signal to the event loop
    connect( & mediaPlayer, & QMediaPlayer::durationChanged, & eventLoop, & QEventLoop::quit);

    mediaPlayer.setSource(QUrl::fromLocalFile(filePath));

    mediaPlayer.play(); // Start playback (needed for duration to be fetched)

    // Start the event loop and wait for the duration to update
    eventLoop.exec();

    // Stop playback after obtaining duration (it's good practice)
    mediaPlayer.stop();

    // Return the duration in milliseconds
    return mediaPlayer.duration();
}

void Filex::processMediaFiles(const QString & directoryPath) {
    QDir dir(directoryPath);

    mediaMap.clear(); // Clear existing data

    QStringList mediaFiles = dir.entryList(QStringList() << "*.mp3" << "*.mp4", QDir::Files);
    QMap < QString, QPair < QString, QString >> mediaMap; // Key: filename, Value: pair of file path and JSON path

    for (const QString & fileName: mediaFiles) {
        QString filePath = dir.filePath(fileName);
        QString jsonFilePath = QFileInfo(filePath).absolutePath() + "/" + fileName.section('.', 0, 0) + ".json";

        if (QFile::exists(jsonFilePath)) {

            jsonAlreadyCreated(jsonFilePath, filePath, fileName);

            getAllMediaNames();

        } else {
            // JSON file does not exist, create one with default values
            qint64 totalLength = getMediaDuration(filePath); // Use the new method for the duration.

            // Check if totalLength is valid
            if (totalLength <= 0) {
                std::cerr << "Error: Could not determine duration for file: " << filePath.toStdString() << std::endl;
                return; // Exit the function early if duration is invalid
            }

            json newJson;
            newJson["Time"] = 0; // Default value
            newJson["total"] = totalLength; // Insert total length

            std::ofstream outFile(jsonFilePath.toStdString());
            if (outFile.is_open()) {
                outFile << newJson.dump(4); // Pretty print JSON
                outFile.close();
                qDebug() << "Created JSON file for:" << fileName;
                mediaMap[fileName] = qMakePair(filePath, jsonFilePath); // Add to the map as well
                getAllMediaNames();
            } else {
                qDebug() << "Failed to create JSON file for:" << fileName;
            }
        }
    }

    // Log the collected data
    for (auto it = mediaMap.begin(); it != mediaMap.end(); ++it) {
        qDebug() << "File:" << it.key() << " | Path:" << it.value().first << " | JSON Path:" << it.value().second;
    }
}

QMap < QString, QPair < QString, QString >> Filex::getMediaMap() const {
    return mediaMap; // Return the media map
}

void Filex::jsonAlreadyCreated(const QString & jsonFilePath,
                               const QString & filePath,
                               const QString & fileName) {
    std::ifstream inFile(jsonFilePath.toStdString());
    if (inFile.is_open()) {
        json loadedJson;
        inFile >> loadedJson; // Load JSON
        inFile.close(); // Close file

        // Access data using key with existence checks
        int time = loadedJson.value("Time", 0); // Default to 0 if not found
        int total = loadedJson.value("total", 0); // Access total length if exists
        if (total > 0) {
            // Calculate percentage complete
            percentage = (static_cast < double > (time) / total) * 100;

            // Log percentage
            std::cout << "File: " << fileName.toStdString() << " | Time: " << time << " | Total: " << total << " | Percentage: " << percentage << "%" << std::endl;

            // Condition to add to map
            if (percentage <= complete) {
                mediaMap[fileName] = qMakePair(filePath, jsonFilePath);
                std::cout << "Added to mediaMap: " << fileName.toStdString() << std::endl;
            } else {
                std::cout << "Skipped " << fileName.toStdString() << " (Completed)" << std::endl;
                skipped++;
            }
        }

        // Print loaded JSON
        std::cout << "Loaded JSON settings from file: " << loadedJson.dump(4) << std::endl; // Pretty print
    } else {
        qDebug() << "Failed to open" << jsonFilePath << "for reading.";
    }

    qDebug() << "JSON file exists for:" << filePath;

}



// Add this method in the Filex class
void Filex::getAllMediaNames() {
    // Iterate over the mediaMap and collect keys (file names)
    QStringList mediaNames;
    for (const auto & key: mediaMap.keys()) {
        mediaNames.append(key);
    }

    // Join the list into a single QString with a separator (e.g., ", ")
    QString mediaNamesString = mediaNames.join(", ");

    // Set the joined string to the label
    ui -> label_6 -> setText(mediaNamesString);

    // For demonstration, we'll print them to the debug output
    for (const QString & name: mediaNames) {
        qDebug() << "Media Name:" << name;
    }

    // Get the count of media names
    int mediaCount = mediaNames.size();
    ui -> label -> setText(QString::number(mediaCount));

    int totalCount = mediaCount + skipped;
    if (totalCount > 1) {
        // Calculate percentage complete
        double percentageComplete = (static_cast < double > (skipped) / totalCount) * 100;
        ui -> label_5 -> setText(QString::number(percentageComplete) + "%");
    } else {
        ui -> label_5 -> setText(QString::number(percentage) + "%");

    }

    // Alternatively, you can return this list for further use
    // return mediaNames; // Uncomment if you want to return it.
}

void Filex::resetMediaMap() {
    mediaMap.clear(); // Clear the existing media map
    skipped = 0;      // Reset the skipped count
    percentage = 0;   // Reset the percentage
    ui->label_6->clear(); // Clear the label displaying media names
    ui->label->setText("0"); // Reset media count label to 0
    ui->label_5->setText("0%"); // Reset percentage label

    qDebug() << "Media map has been reset.";
}



Filex::~Filex() {
    delete ui;
}
