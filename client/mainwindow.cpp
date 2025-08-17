#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QStackedWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    stack = new QStackedWidget(this);
    login = new LoginWindow(this);
    game = new GameWindow(this);

    stack->addWidget(login);
    stack->addWidget(game);

    setCentralWidget(stack);

    stack->setCurrentWidget(login);

    setMinimumSize(1200, 740);

    connect(login, &LoginWindow::loginAttempted, game, &GameWindow::attemptConnection);

    connect(game, &GameWindow::loggedIn, this, [this](){ stack->setCurrentWidget(game); });

    connect(game, &GameWindow::returnToLogin, this, [this](){ stack->setCurrentWidget(login); });
}

MainWindow::~MainWindow() {
    delete ui;
}

