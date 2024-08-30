#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "C://Users//User//Documents//VideoexcesV3fixed//json-develop//json-develop//single_include//nlohmann//json.hpp"
#include "mainwindow.h" // Include the MainWindow header

namespace Ui {
class Settings;
}

class MainWindow; // Forward declaration

class Settings : public QWidget {
    Q_OBJECT

public:
    explicit Settings(MainWindow* mainWindow, QWidget *parent = nullptr);
    ~Settings();


    void loadSettingsFromJson(); // Declaration

public slots:
    void onGoBackButtonClicked();

public:
    Ui::Settings *ui; // Pointer to the UI class
    MainWindow *mainWindow;
};

#endif // SETTINGS_H
