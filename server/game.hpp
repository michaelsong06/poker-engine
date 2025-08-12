#pragma once
#include <vector>
#include <string>
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
    int min_raise;
    int max_raise;
    vector<pair<Player,Action>> history;
    string history_string;
    Evaluator evaluator;
public:

    GameState(int num_players);

    int get_gameNo() const;

    Round get_round() const;
    void next_round();

    int get_pot() const;

    vector<Card>& get_board();
    void deal_to_board(Card new_card);

    vector<Player>& get_players();
    Player& get_current_player();
    Player& get_current_dealer();
    Player& get_sb();
    Player& get_bb();
    void set_dealer_sb_bb();
    void next_player();
    bool get_acted(int index) const;
    void set_acted(int index);
    void reset_acted();

    vector<pair<Player,Action>>& get_history();
    pair<Player,Action> get_last_action() const;
    void update_history(Player player, Action new_action);
    string get_history_string() const;

    Evaluator& get_evaluator();

    vector<int> not_folded() const;

    bool game_end() const;

    bool betting_over() const;
    int make_action(Action new_action);

    void draw_community_cards();

    vector<int> compute_winners_and_distribute_pot();

    void init_new_game();

    void debug_state();
};
