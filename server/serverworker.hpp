#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
using namespace std;

class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)

public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    QString get_username() const;
    void set_username(const QString &userName);
    void sendJson(const QJsonObject &jsonData);

signals:
    void jsonReceived(const QJsonObject &jsonDoc);
    void disconnectedFromClient();
    void error();
    void logMessage(const QString &msg);
public slots:
    void disconnectFromClient();
private slots:
    void receiveJson();
private:
    QTcpSocket *serverSocket;
    QString username;
};
