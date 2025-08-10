#include "server.hpp"

void Server::incomingConnection(qintptr socketDescriptor) {

    ServerWorker *worker = new ServerWorker(this);

    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }

    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&Server::clientDisconnected, this, worker));
    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&Server::jsonReceived, this, worker, std::placeholders::_1));
    connect(worker, &ServerWorker::logMessage, this, &Server::logMessage);

    clients.append(worker);
    emit logMessage(QStringLiteral("New client connected"));

}

void Server::sendJson(ServerWorker *destination, const QJsonObject &msg) {
    Q_ASSERT(destination);
    destination->sendJson(msg);
}

void Server::broadcast(const QJsonObject &msg, ServerWorker *exclude) {
    for (ServerWorker* worker : clients) {
        if (worker == exclude) continue;
        sendJson(worker, msg);
    }
}
