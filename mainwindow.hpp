#pragma once
#include "server/game.hpp"

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

    // Debugging
    void print_game_state();
    void print_round_state();
    void print_players_status();

};
