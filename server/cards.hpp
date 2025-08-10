#pragma once
#include <vector>
#include <string>
using namespace std;

enum Suit { SPADES, HEARTS, DIAMONDS, CLUBS };

enum Rank { TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

class Card {
private:
    Rank rank;
    Suit suit;
public:
    Card(Rank r, Suit s) : rank(r), suit(s) {}
    Suit get_suit();
    Rank get_rank();
    string to_string() const;
    string to_filename() const;
};

class Deck {
private:
    vector<Card> deck;
    int top_index;
public:
    Deck();
    Card draw();
    void burn();
    void reshuffle();
};
