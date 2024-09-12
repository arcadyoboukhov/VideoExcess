#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

#include <QSlider>

#include <QLabel>

#include <QPushButton>

#include <iostream>

#include <fstream>

#include <filesystem>

#include "../include/nlohmann/json.hpp"

#include "mainwindow.h" // Include the MainWindow header

#include <string>

namespace Ui {
class Settings;
}

class MainWindow; // Forward declaration

class Settings: public QWidget {
    Q_OBJECT

public:
    explicit Settings(MainWindow * mainWindow, QWidget * parent = nullptr);
    ~Settings();

    bool isMp3CheckBoxChecked() const;
    int sliderPercentage() const;
    int sliderPlayback() const;

    void loadSettingsFromJson(); // Declaration

    std::string GetConfigLocation() const;

public slots:
    void onGoBackButtonClicked();

public:
    Ui::Settings * ui; // Pointer to the UI class
    MainWindow * mainWindow;
    std::string configLocation;
};

#endif // SETTINGS_H
