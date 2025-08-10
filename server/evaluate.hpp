#pragma once
#include <vector>
#include <string>
#include "cards.hpp"
using namespace std;

enum HandRank { HIGHCARD = 1, PAIR, TWOPAIR, TRIPS, STRAIGHT, FLUSH, FULLHOUSE, QUADS, STRAIGHTFLUSH, ROYALFLUSH };

struct HandScore {
    HandRank type;
    vector<int> tiebreakers;
    // Methods:
    bool operator<(const HandScore& other) const;
    string to_string() const;
};


class Evaluator {
private:
    HandScore evaluate_five_card_hand(vector<Card>& hand);
public:
    HandScore evaluate_table(vector<Card> player, vector<Card> board);
};
