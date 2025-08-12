#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <QObject>
#include <QTimer>
#include "game.hpp"
using namespace std;

enum EngineState {
    IDLE,
    INITGAME,
    STARTROUND,
    PLAYERACTION,
    BETTINGOVER,
    DRAWCARDS,
    DISTRIBUTEPOT
};

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject* parent = nullptr);
    void startGame();

    EngineState get_state();
    void makeAction(const Action& action);
    int get_current_playerID();
    const vector<Player> get_players();
    const Player get_dealer();
    const Player get_sb();
    const Player get_bb();
    int get_game_no();
    Round get_round();
    int get_pot();
    vector<Card> get_board();

signals:
    void gameStateUpdated(const GameState& gameState);
private slots:
    void gameLoop();
private:
    GameState* game;
    EngineState state = IDLE;
    QTimer gameTimer;

    Action pendingAction = Action(FOLD, 0);
    bool actionReady = false;

    void tick();
    void nextState();

    // Debugging
    void print_game_state();
    void print_round_state();
    void print_players_status();
};

#endif // ENGINE_HPP
