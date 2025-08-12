#include "server.hpp"

Server::Server(QObject *parent) : QTcpServer(parent) {}

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


void Server::jsonReceived(ServerWorker *sender, const QJsonObject &doc) {

    Q_ASSERT(sender);

    emit logMessage(QLatin1String("JSON received ") + QString::fromUtf8(QJsonDocument(doc).toJson()));

    const QString type = doc.value(QLatin1String("type")).toString();
    const QJsonObject payload = doc.value(QLatin1String("payload")).toObject();

    if (type == QLatin1String("JOIN_GAME")) {

        const QString username = payload.value(QLatin1String("username")).toString();

        // TODO: player joining game

        QJsonObject message;
        message["type"] = "PLAYER_JOINED";

        QJsonObject payload;
        // payload["player_id"] = 1;
        payload["username"] = username;

        message["payload"] = payload;
        broadcast(message, nullptr);
        return;

    } else if (type == QLatin1String("PLAYER_ACTION")) {

        if (gameEngine->get_state() != PLAYERACTION) return;

        int player_id = payload.value(QLatin1String("player_id")).toInt();
        if (gameEngine->get_current_playerID() != player_id) return;

        unordered_map<string, ActionType> string_to_action = {
            {"CHECK", CHECK},
            {"CALL", CALL},
            {"FOLD", FOLD},
            {"RAISE", RAISE}
        };

        const string action_str = payload.value(QLatin1String("action")).toString().toStdString();
        if (!string_to_action.count(action_str)) return;

        int amount = payload.value(QLatin1String("raise_amt")).toInt();

        Action action = Action(string_to_action[action_str], amount);
        gameEngine->makeAction(action);

        broadcast(doc, nullptr);

    } else if (type == QLatin1String("REQUEST_STATE")) {

        QJsonObject gameState;

        QJsonArray players;
        for (const Player& player : gameEngine->get_players()) {

            QString username = QString::fromStdString(player.get_username());
            int stack = player.get_stack();
            QString role;
            if (player == gameEngine->get_dealer()) {
                role = QString::fromStdString("D");
            } else if (player == gameEngine->get_sb()) {
                role = QString::fromStdString("SB");
            } else if (player == gameEngine->get_bb()) {
                role = QString::fromStdString("BB");
            } else {
                role = QString::fromStdString("None");
            }

            QJsonObject playerObj;
            playerObj[QLatin1String("username")] = username;
            playerObj[QLatin1String("stack")] = stack;
            playerObj[QLatin1String("role")] = role;

            players.append(playerObj);
        }

        gameState[QLatin1String("players")] = players;
        gameState[QLatin1String("game_no")] = gameEngine->get_game_no();
        unordered_map<Round, string> round_to_string = {
            {PREFLOP, "PREFLOP"},
            {FLOP, "FLOP"},
            {TURN, "TURN"},
            {RIVER, "RIVER"}
        };
        gameState[QLatin1String("round")] = QString::fromStdString(round_to_string[gameEngine->get_round()]);
        gameState[QLatin1String("pot")] = gameEngine->get_pot();

        QJsonArray board;
        for (Card card : gameEngine->get_board()) {
            board.append(QString::fromStdString(card.to_string()));
        }
        gameState[QLatin1String("board")] = board;

        gameState[QLatin1String("current_player")] = gameEngine->get_current_playerID();

        sendJson(sender, gameState);
        return;

    } else if (type == QLatin1String("REVEAL_CARDS")) {
        // Received from a specific player (player_id), and is broadcast out to every client

        broadcast(doc, sender);

    }

}

void Server::clientDisconnected(ServerWorker *sender) {
    clients.removeAll(sender);
    const QString username = sender->get_username();
    if (!username.isEmpty()) {
        QJsonObject disconnectedMessage;
        disconnectedMessage[QLatin1String("type")] = QLatin1String("PLAYER_LEFT");

        QJsonObject payload;
        payload[QLatin1String("username")] = username;

        disconnectedMessage[QLatin1String("payload")] = payload;

        broadcast(disconnectedMessage, nullptr);
        emit logMessage(username + QLatin1String(" disconnected"));
    }
    sender->deleteLater();
}

void Server::userError(ServerWorker* sender) {
    Q_UNUSED(sender);
    emit logMessage(QLatin1String("Error from ") + sender->get_username());
}

void Server::stopServer() {
    for (ServerWorker* worker : clients) {
        worker->disconnectFromClient();
    }
    close();
}
