#pragma once

#include <QString>
using namespace std;

/*
MESSAGE JSON FORMATS:

{
    "type": "JOIN_GAME",
    "payload": {
        "name": <username>
    }
}

{
    "type": "PLAYER_ACTION",
    "payload": {
        "action": <action>,
        "amount": <raise_amt>
    }
}

{
    "type": "REQUEST_STATE",
    "payload": {}
}

{
    "type": "ACTION_LOG",
    "payload": {
        "message": <message>
    }
}

{
    "type": "GAME_STATE",
    "payload": {
        "players": [
            {
                "username": <username>,
                "stack": <stack>
            },
            ...
            {
                "username": <username>,
                "stack": <stack>
                }
            }
        ],
        "game_no": <game_no>
        "round": <round>,
        "pot": <pot_amt>,
        "board": [<card>, <card>, ...],
        "current_player": <current_player_index>
    }
}

{
    "type": "PLAYER_JOINED",
    "payload": {
        "player_id": <player_id>,
        "name": <username>
    }
}

{
    "type": "PLAYER_LEFT",
    "payload": {
        "player_id": <player_id>
    }
}

{
    "type": "DEAL_HOLE_CARDS",
    "payload": {
        "cards": [<card>, <card>]
    }
}

{
    "type": "DEAL_COMMUNITY",
    "payload": {
        "round": <round>,
        "board": [<card>, ...]
    }
}

{
    "type": "ROUND_END",
    "payload": {
        "winner_ids": [<player_id>, ...],
        "payout": <winnings>
    }
}

{
    "type": "REVEAL_CARDS",
    "payload": {
        "player_id": <player_id>,
        "cards": [<card>, <card>]
    }
}

{
    "type": "ERROR",
    "payload": {
        "message": <error_msg>
    }
}

*/

enum class MessageType {
    JOIN_GAME,
    PLAYER_ACTION,
    REQUEST_STATE,
    ACTION_LOG,
    GAME_STATE,
    PLAYER_JOINED,
    PLAYER_LEFT,
    DEAL_HOLE_CARDS,
    DEAL_COMMUNITY,
    ROUND_END,
    REVEAL_CARDS,
    ERROR,
    UNKNOWN
};

inline QString messageType_toString(MessageType type) {
    switch (type) {
    case MessageType::JOIN_GAME: return "JOIN_GAME";
    case MessageType::PLAYER_ACTION: return "PLAYER_ACTION";
    case MessageType::REQUEST_STATE: return "REQUEST_STATE";
    case MessageType::ACTION_LOG: return "ACTION_LOG";
    case MessageType::GAME_STATE: return "GAME_STATE";
    case MessageType::PLAYER_JOINED: return "PLAYER_JOINED";
    case MessageType::PLAYER_LEFT: return "PLAYER_LEFT";
    case MessageType::DEAL_HOLE_CARDS: return "DEAL_HOLE_CARDS";
    case MessageType::DEAL_COMMUNITY: return "DEAL_COMMUNITY";
    case MessageType::ROUND_END: return "ROUND_END";
    case MessageType::REVEAL_CARDS: return "REVEAL_CARDS";
    case MessageType::ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}

inline MessageType string_toMessageType(const QString &str) {
    if (str == "JOIN_GAME") return MessageType::JOIN_GAME;
    if (str == "PLAYER_ACTION") return MessageType::PLAYER_ACTION;
    if (str == "REQUEST_STATE") return MessageType::REQUEST_STATE;
    if (str == "ACTION_LOG") return MessageType::ACTION_LOG;
    if (str == "GAME_STATE") return MessageType::GAME_STATE;
    if (str == "PLAYER_JOINED") return MessageType::PLAYER_JOINED;
    if (str == "PLAYER_LEFT") return MessageType::PLAYER_LEFT;
    if (str == "DEAL_HOLE_CARDS") return MessageType::DEAL_HOLE_CARDS;
    if (str == "DEAL_COMMUNITY") return MessageType::DEAL_COMMUNITY;
    if (str == "ROUND_END") return MessageType::ROUND_END;
    if (str == "REVEAL_CARDS") return MessageType::REVEAL_CARDS;
    if (str == "ERROR") return MessageType::ERROR;
    return MessageType::UNKNOWN;
}
