#pragma once

#include <string>
#include <unordered_map>
#include "game.hpp"

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <qDebug>

enum EngineState {
    IDLE,
    INITGAME,
    STARTROUND,
    PLAYERACTION,
    BETTINGOVER,
    DRAWCARDS,
    DISTRIBUTEPOT
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    GameState game;
    EngineState state;

    void tick();

    void player_plays_action();

private slots:

    void on_button_call_clicked();

    void on_button_check_clicked();

    void on_button_fold_clicked();

    void on_button_raise_clicked();

    void gameLoop();

private:
    Ui::MainWindow *ui;

    Action pendingAction;
    bool actionReady = false;

    QTimer* gameTimer;

    void display_card(Card card, QLabel* label);

    void print_game_state() {
        qDebug() << "================|GAME " << game.get_gameNo() << "|================\n";
        qDebug() << "Dealer: Player " << game.get_current_dealer().get_playerID() << "\n";
        qDebug() << "SB: Player " << game.get_sb().get_playerID() << "\n";
        qDebug() << "BB: Player " << game.get_bb().get_playerID() << "\n";
        qDebug() << "========================================\n";
    }

    void print_round_state() {
        unordered_map<Round, string> str_to_enum = {{PREFLOP, "preflop"}, {FLOP, "flop"}, {TURN, "turn"}, {RIVER, "RIVER"}};
        qDebug() << "Round: " << str_to_enum[game.get_round()];
        qDebug() << "Pot: $" << game.get_pot();
        QString board_output = "Board: ";
        for (Card card : game.get_board()) board_output += card.to_string() + " ";
        qDebug().noquote() << board_output;
        qDebug() << "\n";
    }

    void print_players_status() {

        qDebug() << "------------------------PLAYERS STATUS----------------------";
        for (Player& player : game.get_players()) {
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

};
