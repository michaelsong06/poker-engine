#pragma once
#include <vector>
#include "cards.hpp"
using namespace std;

class Player {
private:
    int playerID;
    string username;
    int stack;
    int to_call;
    bool folded;
    vector<Card> hole_cards;
public:
    Player(int newID, string new_username, int init_stack);

    // This is synonymous with getting the player's index within game.players
    int get_playerID() const;

    string get_username() const;
    void set_username(const string &new_username);

    int get_stack() const;

    int bet(int amount);
    void win(int amount);

    int get_to_call() const;
    void set_to_call(int new_to_call);

    void fold();
    bool has_folded() const;
    void unfold();

    void deal_hole_cards(vector<Card> new_hole_cards);
    vector<Card> get_hole_cards() const;

    bool operator==(const Player &other) const;
};
