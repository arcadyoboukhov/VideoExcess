#include "settings.h"
#include "ui_settings.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "C://Users//User//Documents//VideoexcesV3fixed//json-develop//json-develop//single_include//nlohmann//json.hpp"
#include "mainwindow.h" // Include the MainWindow header


using json = nlohmann::json;

void Settings::loadSettingsFromJson() {
    const std::string filePath = "config.json";

    // Print the current working directory
    std::cout << "Current working directory: " << std::filesystem::current_path() << '\n';

    json j = {
        {"mode", false},
        {"playback", 1},
        {"complete", 80}
    };

    // Output the JSON object as a string
    std::cout << j.dump(4) << std::endl; // Pretty print with indentation

    // Save the JSON object to a file
    std::ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << j.dump(4); // Write to file with pretty print
        outFile.close(); // Close the file
        // Print the absolute path of the saved file
        std::cout << "Settings saved to: " << std::filesystem::absolute(filePath) << std::endl;
    } else {
        std::cerr << "Failed to open " << filePath << " for writing." << std::endl;
    }

    // Read the JSON object from the file and print it
    std::ifstream inFile(filePath);
    if (inFile.is_open()) {
        json loadedJson;
        inFile >> loadedJson; // Load the JSON from the file
        inFile.close(); // Close the file

        // Print the loaded JSON
        std::cout << "Loaded JSON settings from file: " << loadedJson.dump(4) << std::endl; // Pretty print
    } else {
        std::cerr << "Failed to open " << filePath << " for reading." << std::endl;
    }

}

Settings::Settings(MainWindow* mainWindow, QWidget *parent) : QWidget(parent), ui(new Ui::Settings), mainWindow(mainWindow) {
    ui->setupUi(this);

    // Connecting UI button signals to slots
    connect(ui->goBackButton, &QPushButton::clicked, this, &Settings::onGoBackButtonClicked);
}

// On back button clicked
void Settings::onGoBackButtonClicked() {
    loadSettingsFromJson(); // Call the function to load
    this->hide(); // Hide the settings window
    mainWindow->show(); // Show the main window
}


Settings::~Settings() {
    delete ui;
}
