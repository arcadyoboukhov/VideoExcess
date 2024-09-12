#include "form.h"

#include "ui_form.h"

Form::Form(MainWindow * mainWindow, QWidget * parent): QWidget(parent), ui(new Ui::Form), mainWindow(mainWindow) {
    ui -> setupUi(this);

    // Connecting UI button signals to slots
    connect(ui -> pushButton, & QPushButton::clicked, this, & Form::onGoBackButtonClicked);
}

// On back button clicked
void Form::onGoBackButtonClicked() {
    this -> hide(); // Hide the settings window
    mainWindow -> show(); // Show the main window
}

Form::~Form() {
    delete ui;
}
