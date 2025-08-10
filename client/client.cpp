#include "client.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
using namespace std;

Client::Client(QObject *parent) : QObject(parent), clientSocket(new QTcpSocket(this)), clientLoggedIn(false) {

    connect(clientSocket, &QTcpSocket::connected, this, &Client::connected);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Client::disconnected);

    connect(clientSocket, &QTcpSocket::readyRead, this, &Client::onReadyRead);

    connect(clientSocket, &QAbstractSocket::errorOccurred, this, &Client::error);

    connect(clientSocket, &QTcpSocket::disconnected, this, &Client::logout);
}

void Client::login(const QString& username) {

    if (clientSocket->state() == QAbstractSocket::ConnectedState) {

        QDataStream clientStream(clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);

        QJsonObject message;
        message["type"] = QStringLiteral("JOIN_GAME");

        QJsonObject payload;
        payload["username"] = username;

        message["payload"] = payload;

        clientStream << QJsonDocument(message).toJson();
    }
}

void Client::logout() {
    clientLoggedIn = false;
}

void Client::makeAction(const QString& actionType, int raise_amt) {

    QDataStream clientStream(clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);

    QJsonObject message;
    message["type"] = QStringLiteral("PLAYER_ACTION");

    QJsonObject payload;
    payload["action"] = actionType;
    payload["amount"] = raise_amt;

    message["payload"] = payload;

    clientStream << QJsonDocument(message).toJson();
}

void Client::disconnectFromHost() {
    clientSocket->disconnectFromHost();
}
void Client::connectToServer(const QHostAddress &address, quint16 port) {
    clientSocket->connectToHost(address, port);
}

void Client::onReadyRead() {

    QByteArray jsonData;
    QDataStream socketStream(clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);

    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    jsonReceived(jsonDoc.object());
            }
        } else break; // read failed, exit loop and wait for more data
    }
}

void Client::jsonReceived(const QJsonObject &doc) {

    const QString type = doc.value(QLatin1String("type")).toString();
    const QJsonObject payload = doc.value(QLatin1String("payload")).toObject();

    if (type == QLatin1String("PLAYER_JOINED")) {
        if (clientLoggedIn) return; // already logged in

        const int player_id = payload.value(QLatin1String("player_id")).toInt();
        const QString username = payload.value(QLatin1String("username")).toString();

        // TODO: Update UI to show new player client joined

        emit loggedIn();
        return;

    } else if (type == QLatin1String("PLAYER_LEFT")) {

        int player_id = payload.value(QLatin1String("player_id")).toInt();

        // TODO: Update UI to show player left
        return

    } else if (type == QLatin1String("GAME_STATE")) {

        const QJsonArray players = payload.value(QLatin1String("players")).toArray();
        for (const QJsonValue &playerVal : players) {
            if (!playerVal.isObject()) continue;
            QJsonObject playerObj = playerVal.toObject();

            QString username = playerObj.value(QLatin1String("username")).toString();
            int stack = playerObj.value(QLatin1String("stack")).toInt();
            QString role = playerObj.value(QLatin1String("role")).toString();

            // TODO: Update UI to show each player's name and stack
        }

        int game_no = payload.value(QLatin1String("game_no")).toInt();
        QString round = payload.value(QLatin1String("round")).toString();
        int pot = payload.value(QLatin1String("pot")).toInt();

        QJsonArray boardArray = payload.value(QLatin1String("board")).toArray();
        QStringList board;
        for (const QJsonValue &cardVal : boardArray) {
            board.append(cardVal.toString());
        }
        int current_player_index = payload.value(QLatin1String("current_player")).toInt();

        // TODO: Update UI to show all these states
        return;

    } else if (type == QLatin1String("DEAL_HOLE_CARDS")) {

        const QJsonArray cards = payload.value(QLatin1String("cards")).toArray();
        QString hole_1 = cards[0].toString();
        QString hole_2 = cards[1].toString();

        // TODO: Update UI to display the hole cards for this specific client
        return;

    } else if (type == QLatin1String("DEAL_COMMUNITY")) {

        const QString round = payload.value(QLatin1String("round")).toString();
        const QJsonArray boardArray = payload.value(QLatin1String("board")).toArray();
        QStringList board;
        for (const QJsonValue &cardVal : boardArray) {
            board.append(cardVal.toString());
        }

        // TODO: Update UI to display new board
        return;

    } else if (type == QLatin1String("ACTION_LOG")) {

        const QString message = payload.value(QLatin1String("message")).toString();

        // TODO: update UI action log to display new message
        return;

    } else if (type == QLatin1String("PLAYER_ACTION")) {

        int player_id = payload.value(QLatin1String("player_id")).toInt();
        const QString action = payload.value(QLatin1String("action")).toString();
        int amount = payload.value(QLatin1String("amount")).toInt();

        // TODO: update UI to reflect new action by player

    } else if (type == QLatin1String("ROUND_END")) {

        const QJsonArray winnersArray = payload.value(QLatin1String("winners")).toArray();
        QList<pair<int,int>> winners;
        for (const QJsonValue &winnerVal : winnersArray) {
            if (!winnerVal.isObject()) continue;
            QJsonObject winnerObj = winnerVal.toObject();
            int winner_id = winnerObj.value(QLatin1String("winner_id")).toInt();
            int payout = winnerObj.value(QLatin1String("payout")).toInt();
            winners.append({winner_id, payout});
        }

        // TODO: update UI to reflect round end winners receiving their money

    }
}




























