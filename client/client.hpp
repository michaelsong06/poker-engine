#pragma once

#include <QMainWindow>
#include <QTcpSocket>
#include <QJsonObject>

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
    void logout();
    void makeAction(const QString &actionType, int raise_amt);
    void disconnectFromHost();
private slots:
    void onReadyRead();
signals:
    void connected();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void actionReceived(int sender_id, const QString &actionType, int raise_amt);
    void error(QAbstractSocket::SocketError socketError);
    void newClientJoined(const QString &username, int player_id);
    void clientLeft(const QString &username, int player_id);
private:
    QTcpSocket* clientSocket;
    bool clientLoggedIn;
    void jsonReceived(const QJsonObject &doc);
};
