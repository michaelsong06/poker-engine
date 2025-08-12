#include "engine.hpp"

Engine::Engine(QObject* parent)
    : QObject(parent)
    , game(new GameState(1)) {

    connect(&gameTimer, &QTimer::timeout, this, &Engine::gameLoop);
    gameTimer.start(1000);

}

void Engine::startGame() {
    if (game->get_players().size() == 1) return; // game cannot start when there is only 1 player
    state = INITGAME;
}

EngineState Engine::get_state() {
    return state;
}

void Engine::makeAction(const Action& action) {
    pendingAction = action;
    actionReady = true;
}

int Engine::get_current_playerID() {
    return game->get_current_player().get_playerID();
}

const vector<Player> Engine::get_players() {
    return game->get_players();
}

const Player Engine::get_dealer() {
    return game->get_current_dealer();
}
const Player Engine::get_sb() {
    return game->get_sb();
}
const Player Engine::get_bb() {
    return game->get_bb();
}
int Engine::get_game_no() {
    return game->get_gameNo();
}
Round Engine::get_round() {
    return game->get_round();
}
int Engine::get_pot() {
    return game->get_pot();
}
vector<Card> Engine::get_board() {
    return game->get_board();
}

void Engine::gameLoop() {
    tick();
    emit gameStateUpdated(*game);
}

void Engine::tick() {
    switch (state) {
    case IDLE:
        break; // do nothing
    case INITGAME:
        print_game_state();
        game->init_new_game();
        qDebug() << "SB has bet $" << SMALLBLIND << "\nBB has bet $" << BIGBLIND << "\n";
        state = STARTROUND;
        print_players_status();
        break;
    case STARTROUND:
        print_round_state();
        state = PLAYERACTION;
        break;
    case PLAYERACTION:
        if (!actionReady) return; // wait for user to press button
        game->make_action(pendingAction);
        actionReady = false;
        if (game->betting_over()) {
            state = BETTINGOVER;
        }
        game->debug_state();
        break;
    case BETTINGOVER:
        if (game->game_end()) {
            state = DISTRIBUTEPOT;
        } else {
            state = DRAWCARDS;
        }
        break;
    case DRAWCARDS:
        game->draw_community_cards();
        game->next_round();
        state = STARTROUND;
        break;
    case DISTRIBUTEPOT:
        vector<int> winners = game->compute_winners_and_distribute_pot();
        state = INITGAME;
        break;
    }
}

// -------------- DEBUGGING FUNCTIONS -----------------------


void Engine::print_game_state() {
    qDebug() << "================|GAME " << game->get_gameNo() << "|================\n";
    qDebug() << "Dealer: Player " << game->get_current_dealer().get_playerID() << "\n";
    qDebug() << "SB: Player " << game->get_sb().get_playerID() << "\n";
    qDebug() << "BB: Player " << game->get_bb().get_playerID() << "\n";
    qDebug() << "========================================\n";
}

void Engine::print_round_state() {
    unordered_map<Round, string> str_to_enum = {{PREFLOP, "preflop"}, {FLOP, "flop"}, {TURN, "turn"}, {RIVER, "RIVER"}};
    qDebug() << "Round: " << str_to_enum[game->get_round()];
    qDebug() << "Pot: $" << game->get_pot();
    QString board_output = "Board: ";
    for (Card card : game->get_board()) board_output += card.to_string() + " ";
    qDebug().noquote() << board_output;
    qDebug() << "\n";
}

void Engine::print_players_status() {

    qDebug() << "------------------------PLAYERS STATUS----------------------";
    for (Player& player : game->get_players()) {
        qDebug() << "Player " << player.get_playerID() << ":";
        qDebug() << "Stack: $" << player.get_stack();
        QString hole_cards_output = "Hole Cards: ";
        for (Card hole_card : player.get_hole_cards()) hole_cards_output += hole_card.to_string() + " ";
        qDebug().noquote() << hole_cards_output;
        qDebug() << "To call: " << player.get_to_call();
        qDebug() << "\n";
    }
    qDebug() << "------------------------------------------------------------";

}
