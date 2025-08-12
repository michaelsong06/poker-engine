#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

#include "serverworker.hpp"
#include "engine.hpp"

using namespace std;

class Server : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(Server)
public:
    explicit Server(QObject* parent = nullptr);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
signals:
    void logMessage(const QString& msg);
public slots:
    void stopServer();
private slots:
    void broadcast(const QJsonObject& msg, ServerWorker *exclude);
    void jsonReceived(ServerWorker *sender, const QJsonObject &doc);
    void clientDisconnected(ServerWorker *client);
    void userError(ServerWorker *client);
private:
    void receiveJson(ServerWorker *sender, const QJsonObject &doc);
    void sendJson(ServerWorker *destination, const QJsonObject &msg);
    QVector<ServerWorker*> clients;
    Engine* gameEngine;
};
