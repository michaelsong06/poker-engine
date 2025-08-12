#pragma once

#include "loginwindow.hpp"
#include "gamewindow.hpp"
#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QLabel>
#include <qDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QStackedWidget* stack;
    LoginWindow* login;
    GameWindow* game;


};
