#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include <QSlider>

#include <QLabel>

#include <QPushButton>

#include "mainwindow.h" // Include the MainWindow header


namespace Ui {
class Form;
}

class MainWindow; // Forward declaration

class Form: public QWidget {
    Q_OBJECT

public:
    explicit Form(MainWindow * mainWindow, QWidget * parent = nullptr);
    ~Form();

public slots:
    void onGoBackButtonClicked();

public:
    Ui::Form * ui;
    MainWindow * mainWindow;
};

#endif // FORM_H
