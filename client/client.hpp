#pragma once

#include <vector>

#include <QMainWindow>
#include <QTcpSocket>
#include <QJsonObject>
#include <QString>
using namespace std;

#define CLIENT_VERSION QDataStream::Version::Qt_5_7

#define SERVER_PORT 1967

#define SERVER_IP "127.0.0.1"

class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)
public:
    explicit Client(QObject* parent = nullptr);
public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void login(const QString &username);
    void set_playerID(int id);
    void logout();
    void makeAction(const QString &actionType, int raise_amt);
    void requestState();
    void disconnectFromHost();
private slots:
    void onReadyRead();
signals:
    void connected();
    void loggedIn(int player_id, const QString& username);
    void loginError(const QString &reason);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void newClientJoined(const QString &username, int player_id);
    void clientLeft(const QString &username, int player_id);

    void playerStateReceived(int player_id, const QString& username, int stack, const QString& role);
    void gameStateReceived(int game_no, int pot, const QStringList& board, int current_player_index, int to_call);
    void actionReceived(int sender_id, const QString& username, const QString &actionType, int to_call, int raise_amt);
    void boardReceived(const QStringList& board);
    void gameLogReceived(const QString& message);
    void winnersReceived(const QList<pair<int,int>>& winners);
    void holeCardsReceived(int player_id, const QString& hole_1, const QString& hole_2);
private:
    QTcpSocket* clientSocket;
    bool clientLoggedIn;
    void jsonReceived(const QJsonObject &doc);
    int playerID = -1;
};
