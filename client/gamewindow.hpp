#pragma once

#include <vector>

#include <QWidget>
#include <QInputDialog>
#include "client.hpp"
#include "ui_gamewindow.h"
using namespace std;

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

    int pot;
    void setPot(int newPot);
    QList<int> player_stacks;
    void updateStack(int playerID, int diff);

    QLineEdit* stackDisplay(int id);
    QLabel* boardDisplay(int index);
    QLineEdit* usernameDisplay(int id);
    QLineEdit* roleDisplay(int id);
    pair<QLabel*, QLabel*> holeCardsDisplay(int id);

    void display_card(const QString& card, QLabel* label);
    QString card_to_filename(const QString& card);
public slots:
    void attemptConnection(const QString &username);
    void connectedToServer();
    void attemptLogin();
    void loggedIn(int player_id, const QString& username);
    void loginFailed(const QString &reason);
signals:
    void returnToLogin();
private slots:
    void playerStateReceived(int player_id, const QString& username, int stack, const QString& role);
    void gameStateReceived(int game_no, int pot, const QStringList& board, int current_player_index, int to_call);
    void actionReceived(int sender_id, const QString& username, const QString &actionType, int to_call, int raise_amt);
    void boardReceived(const QStringList& board);
    void gameLogReceived(const QString& message);
    void winnersReceived(const QList<pair<int, int>>& winners);
    void holeCardsReceived(int player_id, const QString& hole_1, const QString& hole_2);

    void disconnectedFromServer();
    void newClientJoined(const QString &username, int player_id);
    void clientLeft(const QString &username, int player_id);
    void error(QAbstractSocket::SocketError socketError);

};
