#pragma once
#include <stdexcept>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include "cards.hpp"
using namespace std;

enum HandRank { HIGHCARD = 1, PAIR, TWOPAIR, TRIPS, STRAIGHT, FLUSH, FULLHOUSE, QUADS, STRAIGHTFLUSH, ROYALFLUSH };

struct HandScore {
    HandRank type;
    vector<int> tiebreakers;

    bool operator<(const HandScore& other) const {
        if (type != other.type) return type < other.type;
        return tiebreakers < other.tiebreakers;
    }
    string to_string() const {
        static const char* rank_str[] = {"","","2","3","4","5","6","7","8","9","10","Jack","Queen","King","Ace"};
        switch (type) {
        case HIGHCARD:
            return string(rank_str[tiebreakers[0]]) + " High";
        case PAIR:
            return string(rank_str[tiebreakers[0]]) + " Pair | " + string(rank_str[tiebreakers[1]]) + " High Kicker";
        case TWOPAIR:
            return string(rank_str[tiebreakers[0]]) + " " + string(rank_str[tiebreakers[1]]) + " Two-Pair | " + string(rank_str[tiebreakers[2]]) + " High Kicker";
        case TRIPS:
            return "Three of a Kind " + string(rank_str[tiebreakers[0]]) + "s | " + string(rank_str[tiebreakers[1]]) + " High Kicker";
        case STRAIGHT:
            return "Straight | " + string(rank_str[tiebreakers[0]]) + " High";
        case FLUSH:
            return "Flush | " + string(rank_str[tiebreakers[0]]) + " High";
        case FULLHOUSE:
            return string(rank_str[tiebreakers[0]]) + "-" + string(rank_str[tiebreakers[1]]) + " Full House";
        case QUADS:
            return "Four of a Kind " + string(rank_str[tiebreakers[0]]) + "s | " + string(rank_str[tiebreakers[1]]) + " High Kicker";
        case STRAIGHTFLUSH:
            return "Straight Flush | " + string(rank_str[tiebreakers[0]]) + " High";
        case ROYALFLUSH:
            return "Royal Flush";
        }
        return "ERROR HAND NOT RECOGNISED";
    }
};


class Evaluator {
private:

    HandScore evaluate_five_card_hand(vector<Card>& hand) {
        if (hand.size() != 5) throw invalid_argument("Hand size must be 5");

        HandScore result;

        // Get list of ranks and order descending
        vector<int> ranks;
        for (Card& card : hand) ranks.push_back(card.get_rank());
        sort(ranks.begin(), ranks.end(), greater<int>());

        // Check for flush
        Suit cur_suit = hand[0].get_suit();
        bool flush = true;
        for (Card& card : hand) {
            if (card.get_suit() != cur_suit) {
                flush = false;
                break;
            }
        }

        // Check for straight
        bool straight = true;
        for (int i = 0; i < 4 && straight; ++i) {
            if (ranks[i] - 1 != ranks[i + 1] && !(ranks[i] == 14 && ranks[i + 1] == 5)) straight = false;

        }

        // Check for straightflush/royalflush
        if (flush || straight) {
            if (!flush) result.type = STRAIGHT;
            else if (straight) {
                if (ranks[0] == 14 && ranks[1] == 13) result.type = ROYALFLUSH;
                else result.type = STRAIGHTFLUSH;
            } else result.type = FLUSH;

            vector<int> ace_five_straight = {14, 5, 4, 3, 2};
            if (ranks == ace_five_straight) {
                result.tiebreakers = {5, 4, 3, 2, 14};
                return result;
            }

            result.tiebreakers = ranks;
            return result;
        }

        // Create frequency table
        unordered_map<int, int> frequencies;
        for (int rank : ranks) frequencies[rank]++;

        // Gather information on whether quads, trips or pairs exist in this hand
        int quadRank = 0;
        int tripRank = 0;
        vector<int> pairRanks = {};
        vector<int> singleRanks = {};
        for (unordered_map<int, int>::const_iterator it = frequencies.begin(); it != frequencies.end(); ++it) {
            if (it->second == 4) quadRank = it->first;
            else if (it->second == 3) tripRank = it->first;
            else if (it->second == 2) pairRanks.push_back(it->first);
            else singleRanks.push_back(it->first);
        }
        sort(pairRanks.begin(), pairRanks.end(), greater<int>());
        sort(singleRanks.begin(), singleRanks.end(), greater<int>());

        // Check for quads
        if (quadRank != 0) {
            result.type = QUADS;
            result.tiebreakers = {quadRank, singleRanks[0]};

        } else if (tripRank != 0) {

            // Check for full house
            if (pairRanks.size() != 0) {
                result.type = FULLHOUSE;
                result.tiebreakers = {tripRank, pairRanks[0]};

                // Check for trips
            } else {
                result.type = TRIPS;
                result.tiebreakers = {tripRank};
                for (int single : singleRanks) result.tiebreakers.push_back(single);
            }

            // Check for two pair
        } else if (pairRanks.size() == 2) {
            result.type = TWOPAIR;
            result.tiebreakers = pairRanks;
            result.tiebreakers.push_back(singleRanks[0]);

            // Check for pair
        } else if (pairRanks.size() == 1) {
            result.type = PAIR;
            result.tiebreakers = pairRanks;
            for (int single : singleRanks) result.tiebreakers.push_back(single);

            // Not any of the other hand types, so can only be high card
        } else {
            result.type = HIGHCARD;
            result.tiebreakers = singleRanks;
        }

        return result;
    }

public:

    HandScore evaluate_table(vector<Card> player, vector<Card> board) {

        // get the full 7 card hand for each player
        vector<Card> hand = player;
        for (Card card : board) hand.push_back(card);

        // get every 5 card combo for each player
        vector<vector<Card>> handcombos;
        for (int i = 0; i < 7; ++i) {
            for (int j = i + 1; j < 7; ++j) {
                vector<Card> currentcombo;
                for (int cur = 0; cur < 7; ++cur) {
                    if (cur != i && cur != j) {
                        currentcombo.push_back(hand[cur]);
                    }
                }
                handcombos.push_back(currentcombo);
            }
        }

        // Get the best hand out of each set of permutations
        HandScore best = evaluate_five_card_hand(handcombos[0]);
        for (vector<Card> combo : handcombos) best = max(best, evaluate_five_card_hand(combo));

        return best;
    }


};
