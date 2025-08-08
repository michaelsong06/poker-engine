#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <random>
using namespace std;

enum Suit { SPADES, HEARTS, DIAMONDS, CLUBS };

enum Rank { TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

class Card {
private:
    Rank rank;
    Suit suit;
public:
    Card(Rank r, Suit s) : rank(r), suit(s) {}
    Suit get_suit() { return suit; }
    Rank get_rank() { return rank; }
    string to_string() const {
        static const char* suit_str[] = {"C", "D", "H", "S"};
        static const char* rank_str[] = {"","","2","3","4","5","6","7","8","9","T","J","Q","K","A"};
        return string(rank_str[rank]) + suit_str[suit];
    }
    string to_filename() const {
        static const char* suit_str[] = {"clubs", "diamond", "heart", "spade"};
        static const char* rank_str[] = {"","","2","3","4","5","6","7","8","9","10","11","12","13","1"};
        return "card_" + string(suit_str[suit]) + "_" + string(rank_str[rank]) + ".png";
    }
};

class Deck {
private:
    vector<Card> deck;
    int top_index;
public:
    Deck() : top_index(0) {
        deck.reserve(52);
        for (int s = 0; s < 4; ++s) {
            for (int r = 2; r <= 14; ++r) {
                deck.emplace_back(static_cast<Rank>(r), static_cast<Suit>(s));
            }
        }
    }

    Card draw() { return deck[top_index++]; }
    void burn() { top_index++; }

    void reshuffle() {
        random_device rd;
        mt19937 g(rd());
        shuffle(deck.begin(), deck.end(), g);
        top_index = 0;
    }

};
