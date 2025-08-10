#include <algorithm>
#include <qDebug>
#include "player.hpp"
using namespace std;


Player::Player(int newID, int init_stack) : playerID(newID), stack(init_stack), to_call(0), folded(false), hole_cards({}) {}

int Player::get_playerID() const {
    return playerID;
}
int Player::get_stack() const {
    return stack;
}


int Player::bet(int amount) {
    if (stack >= amount) {
        stack -= amount;
        return amount;
    }
    else {
        int all_in = stack;
        stack = 0;
        qDebug() << "ALL IN";
        return all_in;
    }
}
void Player::win(int amount) {
    stack += amount;
}

int Player::get_to_call() const {
    return to_call;
}
void Player::set_to_call(int new_to_call) {
    to_call = min(new_to_call, stack);
}

void Player::fold() {
    folded = true;
    hole_cards.clear();
}
bool Player::has_folded() const {
    return folded;
}
void Player::unfold() {
    folded = false;
}

void Player::deal_hole_cards(vector<Card> new_hole_cards) {
    hole_cards = new_hole_cards;
}
vector<Card> Player::get_hole_cards() const {
    return hole_cards;
}

bool Player::operator==(const Player &other) const {
    return playerID == other.get_playerID();
}
