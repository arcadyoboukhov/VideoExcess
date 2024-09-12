#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include "settings.h"

#include <QApplication>

#include <QLabel>

#include <QPixmap>

#include "form.h"

#include "player.h"

MainWindow::MainWindow(QWidget * parent): QMainWindow(parent), ui(new Ui::mainWindow) {
    ui -> setupUi(this);

    // Load a QImage from a file
    QImage image(":/images/assets/videoexcesslight.png");


    // Check if the image is valid
    if (!image.isNull()) {
        // Convert the QImage to QPixmap
        QPixmap pixmap = QPixmap::fromImage(image);

        // Set the QLabel's pixmap and adjust its size
        ui -> logoLabel -> setPixmap(pixmap);
        ui -> logoLabel -> setScaledContents(false); // Disable scaling contents
        ui -> logoLabel -> resize(pixmap.size()); // Resize the label to fit the pixmap
    } else {
        // Handle the case where the image could not be loaded
        qWarning("Failed to load image.");
    }

    // Create instances of Settings, Form, and Player on the heap
    settingsInstance = new Settings(this);
    formInstance = new Form(this);
    playerInstance = new Player(this);

    // Connect signals and slots
    connect(ui -> pushButton_3, & QPushButton::clicked, this, & MainWindow::hide);
    connect(ui -> pushButton_3, & QPushButton::clicked, settingsInstance, & Settings::show);

    connect(ui -> pushButton_4, & QPushButton::clicked, this, & MainWindow::hide);
    connect(ui -> pushButton_4, & QPushButton::clicked, formInstance, & Form::show);

    connect(ui -> pushButton, & QPushButton::clicked, this, & MainWindow::hide);
    connect(ui -> pushButton, & QPushButton::clicked, playerInstance, & Player::show);
    connect(ui -> pushButton, & QPushButton::clicked, this, [this]() {
        playerInstance -> setVariable(1); // Call setVariable method on playerInstance
    });

    connect(ui -> pushButton_2, & QPushButton::clicked, this, & MainWindow::hide);
    connect(ui -> pushButton_2, & QPushButton::clicked, playerInstance, & Player::show);
    connect(ui -> pushButton_2, & QPushButton::clicked, this, [this]() {
        playerInstance -> setVariable(2); // Call setVariable method on playerInstance
    });
}

MainWindow::~MainWindow() {
    // Delete dynamically allocated instances
    delete settingsInstance;
    delete formInstance;
    delete playerInstance;
    delete ui;
}
