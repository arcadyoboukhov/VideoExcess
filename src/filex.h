#ifndef FILEX_H
#define FILEX_H

#include <QWidget>

#include <QMap>

#include <QString>

#include <QPair>

#include "player.h" // Include the player header

#include "settings.h"

#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

namespace Ui {
class Filex;
}

class Filex: public QWidget {
    Q_OBJECT

public:
    explicit Filex(MainWindow * mainWindow, Player * playerInstance, Settings * settingsInstance, QWidget * parent = nullptr);
    ~Filex();

    QMap < QString, QPair < QString, QString >> getMediaMap() const; // Getter for mediaMap

public slots:
    void onGoBackButtonClicked();
    void onSelectDirectoryButtonClicked();

public:
    void processMediaFiles(const QString & directoryPath);
    void processMediaFile(const QString & directoryPath);
    void jsonAlreadyCreated(const QString & jsonFilePath,
                            const QString & filePath,
                            const QString & fileName); // Update function signature
    int playback = 1;


    int complete;
    void getAllMediaNames();

    Ui::Filex * ui;
    MainWindow * mainWindow;
    Player * playerInstance;
    Settings * settingsInstance;
    void resetMediaMap();

private:
    QMap < QString, QPair < QString, QString >> mediaMap; // Store the media map
    qint64 getMediaDuration(const QString & filePath);
};

#endif // FILEX_H
