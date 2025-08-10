#pragma once

#include <QMainWindow>
#include <QTcpSocket>
#include <QJsonObject>

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
    void actionReceived(const QString &sender);
    void error(QAbstractSocket::SocketError socketError);
private:
    QTcpSocket* clientSocket;
    bool clientLoggedIn;
    void jsonReceived(const QJsonObject &doc);
};
