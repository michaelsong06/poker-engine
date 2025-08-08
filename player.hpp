#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include "cards.hpp"
using namespace std;

class Player {
private:
    int playerID;
    int stack;
    int to_call;
    bool folded;
    vector<Card> hole_cards;
public:
    Player(int newID, int init_stack) : playerID(newID), stack(init_stack), to_call(0), folded(false), hole_cards({}) {}

    // This is synonymous with getting the player's index within game.players
    int get_playerID() const { return playerID; }

    int bet(int amount) {
        if (stack >= amount) {
            stack -= amount;
            return amount;
        }
        else {
            int all_in = stack;
            stack = 0;
            cout << "ALL IN" << "\n";
            return all_in;
        }
    }
    void win(int amount) { stack += amount; }
    int get_stack() const { return stack; }

    int get_to_call() const { return to_call; }
    void set_to_call(int new_to_call) { to_call = min(new_to_call, stack); }

    void fold() {
        folded = true;
        hole_cards.clear();
    }
    bool has_folded() const { return folded; }
    void unfold() { folded = false; }

    void deal_hole_cards(vector<Card> new_hole_cards) { hole_cards = new_hole_cards; }
    vector<Card> get_hole_cards() const { return hole_cards; }

    bool operator==(const Player &other) const {
        return playerID == other.get_playerID();
    }
};
