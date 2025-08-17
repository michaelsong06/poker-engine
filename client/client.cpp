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

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::error);
#else
    connect(clientSocket, &QAbstractSocket::errorOccurred, this, &Client::error);
#endif

    connect(clientSocket, &QTcpSocket::disconnected, this, &Client::logout);
}

void Client::login(const QString& username) {

    if (clientSocket->state() == QAbstractSocket::ConnectedState) {

        QDataStream clientStream(clientSocket);
        clientStream.setVersion(CLIENT_VERSION);

        QJsonObject message;
        message["type"] = QStringLiteral("JOIN_GAME_REQUEST");

        QJsonObject payload;
        payload["username"] = username;

        message["payload"] = payload;

        clientStream << QJsonDocument(message).toJson();
    }
}

void Client::set_playerID(int id) {
    playerID = id;
}

void Client::logout() {
    clientLoggedIn = false;
}

void Client::makeAction(const QString& actionType, int raise_amt) {

    QDataStream clientStream(clientSocket);
    clientStream.setVersion(CLIENT_VERSION);

    QJsonObject message;
    message["type"] = QStringLiteral("PLAYER_ACTION");

    QJsonObject payload;
    payload["action"] = actionType;
    payload["amount"] = raise_amt;

    message["payload"] = payload;

    clientStream << QJsonDocument(message).toJson();
}

void Client::requestState() {
    QDataStream clientStream(clientSocket);
    clientStream.setVersion(CLIENT_VERSION);

    QJsonObject message;
    message["type"] = QStringLiteral("REQUEST_STATE");
    QJsonObject payload;
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
    socketStream.setVersion(CLIENT_VERSION);

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

    if (type == QLatin1String("JOIN_GAME_ACCEPT")) {
        if (clientLoggedIn) return; // already logged in

        const int player_id = payload.value(QLatin1String("player_id")).toInt();
        const QString username = payload.value(QLatin1String("username")).toString();

        emit loggedIn(player_id, username);

        requestState();
        return;

    } else if (type == QLatin1String("PLAYER_JOINED")) {

        const int player_id = payload.value(QLatin1String("player_id")).toInt();
        const QString username = payload.value(QLatin1String("username")).toString();

        emit newClientJoined(username, player_id);
        return;

    } else if (type == QLatin1String("PLAYER_LEFT")) {

        const int player_id = payload.value(QLatin1String("player_id")).toInt();
        const QString username = payload.value(QLatin1String("username")).toString();

        emit clientLeft(username, player_id);
        return;

    } else if (type == QLatin1String("GAME_STATE")) {

        const QJsonArray players = payload.value(QLatin1String("players")).toArray();
        for (const QJsonValue &playerVal : players) {
            if (!playerVal.isObject()) continue;
            QJsonObject playerObj = playerVal.toObject();

            int player_id = playerObj.value(QLatin1String("player_id")).toInt();
            QString username = playerObj.value(QLatin1String("username")).toString();
            int stack = playerObj.value(QLatin1String("stack")).toInt();
            QString role = playerObj.value(QLatin1String("role")).toString();

            emit playerStateReceived(player_id, username, stack, role);
        }

        int game_no = payload.value(QLatin1String("game_no")).toInt();
        int pot = payload.value(QLatin1String("pot")).toInt();
        QJsonArray boardArray = payload.value(QLatin1String("board")).toArray();
        QStringList board;
        for (const QJsonValue &cardVal : boardArray) {
            board.append(cardVal.toString());
        }
        int current_player_index = payload.value(QLatin1String("current_player")).toInt();

        int to_call = payload.value(QLatin1String("to_call")).toInt();

        // TODO: Update UI to show all these states
        emit gameStateReceived(game_no, pot, board, current_player_index, to_call);
        return;

    } else if (type == QLatin1String("DEAL_HOLE_CARDS")) {

        const QJsonArray cards = payload.value(QLatin1String("cards")).toArray();
        QString hole_1 = cards[0].toString();
        QString hole_2 = cards[1].toString();

        // TODO: Update UI to display the hole cards for this specific client
        emit holeCardsReceived(playerID, hole_1, hole_2);
        return;

    } else if (type == QLatin1String("DEAL_COMMUNITY")) {

        const QString round = payload.value(QLatin1String("round")).toString();
        const QJsonArray boardArray = payload.value(QLatin1String("board")).toArray();
        QStringList board;
        for (const QJsonValue &cardVal : boardArray) {
            board.append(cardVal.toString());
        }

        emit boardReceived(board);
        return;

    } else if (type == QLatin1String("ACTION_LOG")) {

        const QString message = payload.value(QLatin1String("message")).toString();

        emit gameLogReceived(message);
        return;

    } else if (type == QLatin1String("PLAYER_ACTION")) {

        int player_id = payload.value(QLatin1String("player_id")).toInt();
        const QString username = payload.value(QLatin1String("username")).toString();
        const QString action = payload.value(QLatin1String("action")).toString();
        int to_call = payload.value(QLatin1String("to_call")).toInt();
        int raise_amt = payload.value(QLatin1String("raise_amt")).toInt();

        emit actionReceived(player_id, username, action, to_call, raise_amt);
        return;

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

        emit winnersReceived(winners);
        return;

    } else if (type == QLatin1String("REVEAL_CARDS")) {

        int player_id = payload.value(QLatin1String("player_id")).toInt();
        QJsonArray cardsArray = payload.value(QLatin1String("cards")).toArray();
        QString hole_1 = cardsArray[0].toString();
        QString hole_2 = cardsArray[1].toString();

        emit holeCardsReceived(player_id, hole_1, hole_2);

    } else if (type == QLatin1String("ERROR")) {

        const QString message = payload.value(QLatin1String("message")).toString();
        emit gameLogReceived(message);

        requestState();
        return;

    }
}



























