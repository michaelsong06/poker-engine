#include "loginwindow.hpp"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    connect(ui->playButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);

    setMinimumSize(1200, 740);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}


void LoginWindow::handleLogin() {
    QString username = ui->usernameEdit->text().trimmed();
    if (!username.isEmpty()) {
        emit loginAttempted(username);
    }
}
