#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settings.h"
#include <QApplication>
#include <QLabel>
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settingsInstance = new Settings(this); // Create the Settings instance

    QLabel label;
    // Load a QPixmap from a file
    QPixmap pixmap("C://Users//User//Documents//VideoexcesV3fixed//videoexcess.png"); // Provide the correct path to your image

    // Set the pixmap to the label
    label.setPixmap(pixmap);

    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::hide);
    connect(ui->pushButton_3, &QPushButton::clicked, settingsInstance, &Settings::show);
}

MainWindow::~MainWindow() {
    delete settingsInstance; // Clean up the Settings instance
    delete ui;
}
