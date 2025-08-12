#pragma once

#include <QWidget>
#include <QInputDialog>
#include "client.hpp"

namespace Ui {
class GameWindow;
}

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();
private:
    Ui::GameWindow *ui;
    Client *clientPlaying;
    QString username;
public slots:
    void attemptConnection(const QString &username);
    void connectedToServer();
    void attemptLogin();
    void loggedIn();
    void loginFailed(const QString &reason);
    void returnToLogin();
private slots:
    void actionReceived(int sender_id, const QString &actionType, int raise_amt);
    void call();
    void check();
    void fold();
    void raise();
    void disconnectedFromServer();
    void newClientJoined(const QString &username, int player_id);
    void clientLeft(const QString &username, int player_id);
    void error(QAbstractSocket::SocketError socketError);

};
