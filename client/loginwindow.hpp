#pragma once

#include <QWidget>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void loginAttempted(const QString &username);

private slots:
    void handleLogin();

private:
    Ui::LoginWindow *ui;
};
