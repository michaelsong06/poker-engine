#include "serverworker.hpp"

#include <QOverload>

ServerWorker::ServerWorker(QObject *parent) : QObject(parent), serverSocket(new QTcpSocket(this)) {

    connect(serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::receiveJson);

    connect(serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
    connect(serverSocket, &QAbstractSocket::errorOccurred, this, &ServerWorker::error);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor) {
    return serverSocket->setSocketDescriptor(socketDescriptor);
}
void ServerWorker::disconnectFromClient() {
    serverSocket->disconnectFromHost();
}

QString ServerWorker::get_username() const {
    return username;
}

void ServerWorker::set_username(const QString &new_username) {
    username = new_username;
}

void ServerWorker::receiveJson() {

    QByteArray jsonData;
    QDataStream socketStream(serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);

    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()) {
                    emit jsonReceived(jsonDoc.object());
                } else {
                    emit logMessage("Invalid json received: " + QString::fromUtf8(jsonData));
                }
            } else {
                emit logMessage("Invalid json received: " + QString::fromUtf8(jsonData));
            }
        } else break;
    }
}

void ServerWorker::sendJson(const QJsonObject &json) {

    const QByteArray jsonData = QJsonDocument(json).toJson();
    emit logMessage("Sending to " + username + ": " + QString::fromUtf8(jsonData));
    QDataStream socketStream(serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;

}
