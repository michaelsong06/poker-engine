#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <qDebug>
#include "cards.hpp"
#include "evaluate.hpp"
#include "player.hpp"

#include <QApplication>
using namespace std;

#define SMALLBLIND 1
#define BIGBLIND 2
#define INITIALSTACK 200

#define MAXPLAYERS 6

enum Round { PREFLOP, FLOP, TURN, RIVER };

enum ActionType { FOLD, CALL, RAISE, CHECK };

struct Action {
    ActionType type;
    int amount; // for raise only
    Action(ActionType new_type, int new_amount) : type(new_type), amount(new_amount) {}
};

class GameState {
private:
    int gameNo;
    Round round;
    int pot;
    vector<Card> community_cards;
    Deck deck;
    vector<Player> players;
    int current_player_index;
    int dealer_index;
    int sb_index;
    int bb_index;
    int last_raiser_index;
    int last_to_act_index;
    vector<bool> acted;
    vector<pair<Player,Action>> history;
    string history_string;

public:

    Evaluator evaluator;

    GameState(int num_players) {
        gameNo = 0;
        round = PREFLOP;
        pot = 0;
        community_cards = {};
        deck = Deck();
        for (int i = 0; i < num_players; ++i) players.emplace_back(i, INITIALSTACK);
        current_player_index = -1;
        dealer_index = -1;
        sb_index = -1;
        bb_index = -1;
        last_raiser_index = -1;
        last_to_act_index = 0;
        acted = vector<bool>(players.size(), false);
        history = {};
        history_string = "";
        evaluator = Evaluator();
    }

    int get_gameNo() const { return gameNo; }

    Round get_round() const { return round; }

    void next_round() {
        round = static_cast<Round>((round + 1) % 4);
        current_player_index = (dealer_index + 1) % players.size();
        while (get_current_player().has_folded()) current_player_index++;
        last_to_act_index = current_player_index;
        last_raiser_index = -1;

        reset_acted();

        unordered_map<Round, string> str_to_enum = {{PREFLOP, "preflop"}, {FLOP, "flop"}, {TURN, "turn"}, {RIVER, "RIVER"}};
        qDebug() << "================|NEW ROUND " << str_to_enum[get_round()]  << "|================";
    }

    int get_pot() const { return pot; }

    vector<Card>& get_board() { return community_cards; }
    void deal_to_board(Card new_card) { if (community_cards.size() < 5) community_cards.push_back(new_card); }

    vector<Player>& get_players() { return players; }
    Player& get_current_player() { return players[current_player_index]; }
    Player& get_current_dealer() { return players[dealer_index]; }
    Player& get_sb() { return players[sb_index]; }
    Player& get_bb() { return players[bb_index]; }
    void set_dealer_sb_bb() {
        do { dealer_index = (dealer_index + 1) % players.size(); } while (players[dealer_index].has_folded());

        sb_index = (dealer_index + 1) % players.size();
        while (players[sb_index].has_folded()) sb_index = (sb_index + 1) % players.size();

        bb_index = (sb_index + 1) % players.size();
        while (players[bb_index].has_folded()) bb_index = (bb_index + 1) % players.size();
    }
    void next_player() {
        do { current_player_index = (current_player_index + 1) % players.size(); }
        while (get_current_player().has_folded());
    }

    bool get_acted(int index) const { return acted[index]; }
    void set_acted(int index) { acted[index] = true; }
    void reset_acted() { for (int i = 0; i < int(acted.size()); ++i) { if (!players[i].has_folded()) acted[i] = false; } }

    vector<pair<Player,Action>>& get_history() { return history; }
    pair<Player,Action> get_last_action() const { return history.back(); }
    void update_history(Player player, Action new_action) {
        history.emplace_back(player, new_action);

        // This is updating the history_string which will be displayed on the GUI
        static unordered_map<ActionType,string> action_to_string = {
            {CHECK, "checks"},
            {CALL, "calls"},
            {FOLD, "folds"},
            {RAISE, "raises"}
        };
        string action_message;
        if (!history_string.empty()) action_message += "\n";
        action_message += "> Player " + to_string(player.get_playerID()) + " " + action_to_string[new_action.type];
        if (new_action.type == RAISE) {
            if (history.size() > 1) action_message += " another";
            action_message += " $" + to_string(new_action.amount);
        }
        history_string += action_message;
    }
    string get_history_string() const { return history_string; }

    Evaluator& get_evaluator() { return evaluator; }

    vector<int> not_folded() const {
        vector<int> still_in;
        for (int i = 0; i < int(players.size()); ++i) {
            if (!players[i].has_folded()) still_in.push_back(i);
        }
        return still_in;
    }

    bool game_end() const {
        if (not_folded().size() == 1) return true;
        if (round == RIVER && betting_over()) return true;
        return false;
    }

    bool betting_over() const {
        if (not_folded().size() == 1) return true;
        for (int i = 0; i < int(players.size()); ++i) {
            if (!players[i].has_folded() && !get_acted(i)) return false;
        }
        if (last_raiser_index == -1) return current_player_index == last_to_act_index;

        int player_before_index = (current_player_index - 1 + players.size()) % players.size();
        while (players[player_before_index].has_folded()) player_before_index = (player_before_index - 1 + players.size()) % players.size();
        return player_before_index == last_to_act_index;
    }

    int make_action(Action new_action) {

        Player& player = get_current_player();
        bool all_in = false;

        switch (new_action.type) {
        case FOLD:
            player.fold();
            while (players[last_to_act_index].has_folded()) last_to_act_index = (last_to_act_index - 1) + players.size() % players.size();
            break;
        case CALL:
            pot += player.bet(player.get_to_call());
            player.set_to_call(0);
            break;
        case RAISE:
            if (new_action.amount >= player.get_stack()) all_in = true;
            pot += player.bet(player.get_to_call() + new_action.amount);
            player.set_to_call(0);
            last_raiser_index = current_player_index;
            last_to_act_index = (current_player_index - 1 + players.size()) % players.size();
            while (players[last_to_act_index].has_folded()) last_to_act_index = (last_to_act_index - 1 + players.size()) % players.size();
            reset_acted();
            for (int i = 0; i < int(players.size()); ++i) {
                if (i != current_player_index && !players[i].has_folded()) {
                    players[i].set_to_call(players[i].get_to_call() + new_action.amount);
                }
            }
            break;
        case CHECK:
            if (player.get_to_call() != 0) {
                qDebug() << "Cannot check, please call or raise";
                return -1;
            }
            break;
        default:
            throw invalid_argument("Invalid action type");
        }
        set_acted(current_player_index);

        update_history(player, new_action);
        if (all_in) history_string += "\n   ALL IN";
        next_player();
        return 0;
    }

    void draw_community_cards() {
        switch (round) {
        case PREFLOP:
            deck.burn();
            for (int i = 0; i < 3; ++i) {
                deal_to_board(deck.draw());
            }
            break;
        case FLOP:
        case TURN:
            deck.burn();
            deal_to_board(deck.draw());
            break;
        case RIVER:
            qDebug() << "No more cards to draw onto table";
            break;
        default:
            throw invalid_argument("Invalid round");
        }
    }

    vector<int> compute_winners_and_distribute_pot() {

        vector<int> still_in = not_folded();

        if (still_in.size() > 1 && community_cards.size() == 5) {
            for (int index : still_in) {
                qDebug() << "Player " << players[index].get_playerID() << ": "
                         << evaluator.evaluate_table(players[index].get_hole_cards(), get_board()).to_string();
            }
        }

        vector<pair<int, HandScore>> evaluated;
        HandScore best_score;

        for (int index : still_in) {
            HandScore score = get_evaluator().evaluate_table(players[index].get_hole_cards(), get_board());
            evaluated.emplace_back(index, score);

            if (evaluated.size() == 1 || best_score < score) {
                best_score = score;
            }
        }

        vector<int> winners;
        for (const auto& [index, score] : evaluated) {
            if (!(score < best_score) && !(best_score < score)) { // i.e. score == best_score
                winners.push_back(index);
            }
        }

        if (winners.size() == 1) qDebug() << "Winner:";
        else qDebug() << "Winners:";

        int winnings = get_pot() / winners.size();
        int total = get_pot();
        string win_message = "";

        for (int index : winners) {
            total -= winnings;
            int overall_winnings;
            if (total > 0 && total < winnings) overall_winnings = winnings + total; // handling remainders if not even split
            else overall_winnings = winnings;
            players[index].win(overall_winnings);
            win_message += "\n> Player " + to_string(get_players()[index].get_playerID()) + " wins $" + to_string(overall_winnings);
            if (still_in.size() > 1 && community_cards.size() == 5) {
                win_message += " with " + evaluator.evaluate_table(get_players()[index].get_hole_cards(), get_board()).to_string();
            }
        }
        qDebug() << win_message;
        history_string += win_message;

        return winners;
    }

    void init_new_game() {

        gameNo++;
        round = PREFLOP;
        pot = 0;
        community_cards = {};
        history.clear();
        reset_acted();

        for (Player& player: players) {
            if (player.get_stack() == 0) player.fold();
            else {
                player.set_to_call(0);
                player.unfold();
            }
        }

        int players_still_in = 0;
        for (Player& player: players) {
            if (!player.has_folded()) players_still_in++;
        }
        if (players_still_in < 2) {
            qDebug() << "We have a winner!";
            QApplication::quit();
        }

        set_dealer_sb_bb();
        current_player_index = (dealer_index + 1) % players.size();
        while (players[current_player_index].has_folded()) current_player_index = (dealer_index + 1) % players.size();
        last_raiser_index = -1;
        last_to_act_index = dealer_index;

        if (gameNo > 1) history_string += "\n";
        history_string += "> -----Game " + to_string(gameNo) + "-----";

        // SB and BB make starting bets
        make_action(Action(RAISE, static_cast<int>(SMALLBLIND)));
        make_action(Action(RAISE, static_cast<int>(BIGBLIND - SMALLBLIND)));
        last_to_act_index = get_bb().get_playerID(); // Give BB chance to check/raise again

        // Reinitialize deck and deal hole cards to players
        deck.reshuffle();
        for (Player& player : players) {
            if (player.get_stack() > 0 && !player.has_folded()) {
                vector<Card> hole_cards = { deck.draw(), deck.draw() };
                player.deal_hole_cards(hole_cards);
            }
        }
    }

    void debug_state() {
        qDebug() << "Current Player Index: " << current_player_index;
        qDebug() << "Last Raiser Index: " << last_raiser_index;
        qDebug() << "Last To Act Index: " << last_to_act_index;
        QString acted_output = "Acted: ";
        for (int i = 0; i < int(acted.size()); ++i) {
            acted_output += (acted[i] ? "1" : "0");
            acted_output += " ";
        }
        qDebug().noquote() << acted_output;
        qDebug() << "\n";
    }
};
