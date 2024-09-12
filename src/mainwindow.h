#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class mainWindow;
}
QT_END_NAMESPACE

class Settings; // Forward declaration
class Form;
class Player;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

public:
    Ui::mainWindow *ui;
    Settings *settingsInstance; // Make sure this is declared for deletion in the destructor
    Form *formInstance;
    Player *playerInstance;
};

#endif // MAINWINDOW_H
