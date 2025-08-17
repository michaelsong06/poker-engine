#include "gamewindow.hpp"
#include "ui_gamewindow.h"

#include <QmessageBox>
#include <QGraphicsDropShadowEffect>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWindow)
    , clientPlaying(new Client(this))
{
    ui->setupUi(this);

    connect(clientPlaying, &Client::connected, this, &GameWindow::connectedToServer);
    connect(clientPlaying, &Client::loggedIn, this, &GameWindow::loggedIn);
    connect(clientPlaying, &Client::loginError, this, &GameWindow::loginFailed);
    connect(clientPlaying, &Client::disconnected, this, &GameWindow::disconnectedFromServer);
    connect(clientPlaying, &Client::error, this, &GameWindow::error);
    connect(clientPlaying, &Client::newClientJoined, this, &GameWindow::newClientJoined);
    connect(clientPlaying, &Client::clientLeft, this, &GameWindow::clientLeft);

    connect(clientPlaying, &Client::playerStateReceived, this, &GameWindow::playerStateReceived);
    connect(clientPlaying, &Client::gameStateReceived, this, &GameWindow::gameStateReceived);
    connect(clientPlaying, &Client::actionReceived, this, &GameWindow::actionReceived);
    connect(clientPlaying, &Client::boardReceived, this, &GameWindow::boardReceived);
    connect(clientPlaying, &Client::gameLogReceived, this, &GameWindow::gameLogReceived);
    connect(clientPlaying, &Client::winnersReceived, this, &GameWindow::winnersReceived);

    connect(ui->button_call, &QPushButton::clicked, this, [=]() { clientPlaying->makeAction("CALL", 0); });
    connect(ui->button_check, &QPushButton::clicked, this, [=]() { clientPlaying->makeAction("CHECK", 0); });
    connect(ui->button_fold, &QPushButton::clicked, this, [=]() { clientPlaying->makeAction("FOLD", 0); });
    connect(ui->button_raise, &QPushButton::clicked, this, [=]() {
        int raise_amt = ui->slider_raise->value();
        clientPlaying->makeAction("RAISE", raise_amt);
    });
    connect(ui->slider_raise, &QSlider::valueChanged, this, [=](int value) {
        ui->display_raiseamt->setText("$" + QString::number(value));
    });
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::attemptConnection(const QString &username) {

    // save username since we lose it after entering game window
    this->username = username;

    const QString hostAddress = SERVER_IP; // LOCALHOST
    int hostPort = SERVER_PORT; // PORT

    clientPlaying->connectToServer(QHostAddress(hostAddress), hostPort);

}

void GameWindow::connectedToServer() {
    attemptLogin();
}

void GameWindow::attemptLogin() {
    clientPlaying->login(username);
}

void GameWindow::loggedIn(int player_id, const QString& username) {
    clientPlaying->set_playerID(player_id);
    newClientJoined(username, player_id);
}

void GameWindow::loginFailed(const QString &reason) {
    QMessageBox::critical(this, tr("Error"), reason);
    emit returnToLogin();
}

void GameWindow::disconnectedFromServer() {
    QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));
    emit returnToLogin();
}

void GameWindow::newClientJoined(const QString &username, int player_id) {
    usernameDisplay(player_id)->setText(username);
    gameLogReceived(username + " has joined the game");
}

void GameWindow::clientLeft(const QString &username, int player_id) {
    usernameDisplay(player_id)->clear();
    gameLogReceived(username + " has left the game");
    pair<QLabel*, QLabel*> hole_cards = holeCardsDisplay(player_id);
    hole_cards.first->clear();
    hole_cards.second->clear();
}

void GameWindow::error(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        return; // handled by disconnectedFromServer
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The host refused the connection"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The proxy refused the connection"));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the proxy"));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the server"));
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, tr("Error"), tr("You don't have permissions to execute this operation"));
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, tr("Error"), tr("Too many connections opened"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(this, tr("Error"), tr("Operation timed out"));
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy timed out"));
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, tr("Error"), tr("Unable to reach the network"));
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, tr("Error"), tr("An unknown error occured"));
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, tr("Error"), tr("Operation not supported"));
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, tr("Error"), tr("Your proxy requires authentication"));
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy comunication failed"));
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));
        return;
    default:
        Q_UNREACHABLE();
    }
    emit returnToLogin();
}

QLineEdit* GameWindow::stackDisplay(int id) {
    static QLineEdit* stack_displays[] = {
        ui->display_stack0,
        ui->display_stack1,
        ui->display_stack2,
        ui->display_stack3,
        ui->display_stack4,
        ui->display_stack5,
    };
    return stack_displays[id];
}

QLabel* GameWindow::boardDisplay(int index) {
    static QLabel* boardLabels[] = {
        ui->board_flop1,
        ui->board_flop2,
        ui->board_flop3,
        ui->board_turn,
        ui->board_river
    };
    return boardLabels[index];
}

QLineEdit* GameWindow::usernameDisplay(int id) {
    static QLineEdit* username_displays[] = {
        ui->username_0,
        ui->username_1,
        ui->username_2,
        ui->username_3,
        ui->username_4,
        ui->username_5,
    };
    return username_displays[id];
}

QLineEdit* GameWindow::roleDisplay(int id) {
    static QLineEdit* role_displays[] = {
        ui->role_p0,
        ui->role_p1,
        ui->role_p2,
        ui->role_p3,
        ui->role_p4,
        ui->role_p5,
    };
    return role_displays[id];
}

pair<QLabel*, QLabel*> GameWindow::holeCardsDisplay(int id) {
    static pair<QLabel*,QLabel*> hole_displays[] = {
        {ui->p0_hole1, ui->p0_hole2},
        {ui->p1_hole1, ui->p1_hole2},
        {ui->p2_hole1, ui->p2_hole2},
        {ui->p3_hole1, ui->p3_hole2},
        {ui->p4_hole1, ui->p4_hole2},
        {ui->p5_hole1, ui->p5_hole2},
    };
    return hole_displays[id];
}

void GameWindow::setPot(int newPot) {
    pot = newPot;
    ui->display_pot->setText("$" + QString::number(pot));
}

void GameWindow::updateStack(int playerID, int diff) {
    player_stacks[playerID] += diff;
    stackDisplay(playerID)->setText("$" + QString::number(player_stacks[playerID]));
}

void GameWindow::display_card(const QString& card, QLabel* label) {

    QString path = ":/assets/" + card_to_filename(card);
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qWarning() << "Failed to load card image:" << path;
        label->clear();  // Fallback: clear the label if image load fails
        return;
    }
    label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setScaledContents(true); // Optional: ensures full scaling behavior
}

QString GameWindow::card_to_filename(const QString& card) {
    Q_ASSERT(card.length() == 2);
    static const QMap<QChar, QString> suit_str = {{'C',"clubs"}, {'D',"diamond"}, {'H',"heart"}, {'S',"spade"}};

    static const QString ranks = "#A23456789TJQK";
    int rankIndex = ranks.indexOf(card[0]);
    Q_ASSERT(rankIndex != -1);

    return QString("card_%1_%2.png").arg(suit_str[card[1]]).arg(rankIndex);
}

void GameWindow::playerStateReceived(int player_id, const QString& username, int stack, const QString& role) {

    usernameDisplay(player_id)->setText(username);
    player_stacks[player_id] = stack;
    updateStack(player_id, 0);
    if (role == "None") {
        roleDisplay(player_id)->clear();
    } else {
        roleDisplay(player_id)->setText(role);
    }

}

void GameWindow::gameStateReceived(int game_no, int newPot, const QStringList& board, int current_player_index, int to_call) {

    ui->display_gameno->setText(QString::number(game_no));
    setPot(newPot);
    boardReceived(board);

    auto* glow = new QGraphicsDropShadowEffect;
    glow->setBlurRadius(30); // Adjust glow softness
    glow->setOffset(0);      // Centered glow
    glow->setColor(QColor(255, 215, 0)); // Gold glow
    for (int i = 0; i < 6; ++i) {
        if (i == current_player_index) {
            roleDisplay(i)->setGraphicsEffect(glow);
        } else {
            roleDisplay(i)->setGraphicsEffect(nullptr);
        }
    }
    ui->display_action_on->setText(QString::number(current_player_index));
    ui->display_tocall->setText("$" + QString::number(to_call));

}

void GameWindow::actionReceived(int sender_id, const QString& username, const QString &actionType, int to_call, int raise_amt) {


    if (actionType == "CALL") {
        updateStack(sender_id, -to_call);
        setPot(pot + to_call);
        gameLogReceived(username + " calls");
    } else if (actionType == "CHECK") {
        gameLogReceived(username + " checks");
    } else if (actionType == "FOLD") {
        pair<QLabel*, QLabel*> folded_holes = holeCardsDisplay(sender_id);
        folded_holes.first->clear();
        folded_holes.second->clear();
        gameLogReceived(username + " folds");
    } else { // RAISE
        updateStack(sender_id, -to_call - raise_amt);
        setPot(pot + to_call + raise_amt);
        gameLogReceived(username + " raises $" + QString::number(raise_amt));
    }

}

void GameWindow::boardReceived(const QStringList& board) {

    for (int i = 0; i < board.size(); ++i) {
        display_card(board[i], boardDisplay(i));
    }

}

void GameWindow::gameLogReceived(const QString& message) {
    ui->display_action->append(message + QString::fromStdString("\n"));
    ui->display_action->moveCursor(QTextCursor::End);
}

void GameWindow::winnersReceived(const QList<pair<int, int>>& winners) {

    for (pair<int, int> winner : winners) {
        updateStack(winner.first, winner.second);
    }

}

void GameWindow::holeCardsReceived(int player_id, const QString& hole_1, const QString& hole_2) {

    pair<QLabel*, QLabel*> hole_card_displays = holeCardsDisplay(player_id);
    display_card(hole_1, hole_card_displays.first);
    display_card(hole_2, hole_card_displays.second);

}

/*
#include <vector>
#include <string>
#include <unordered_map>

#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QLayout>
#include <QString>
#include <QDir>
#include <QPalette>
#include <QGraphicsDropShadowEffect>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#define NUM_PLAYERS 6

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    game(GameState(NUM_PLAYERS)),
    state(INITGAME),
    ui(new Ui::MainWindow),
    pendingAction(Action(RAISE,0)),
    gameTimer(new QTimer(this))
{
    ui->setupUi(this);

    connect(ui->slider_raise, &QSlider::valueChanged, this, [=](int value) {
        ui->display_raiseamt->setText("$" + QString::number(value));
    });

    connect(gameTimer, &QTimer::timeout, this, &MainWindow::gameLoop);
    gameTimer->start(100);

    QDir resourceDir(":/assets");
    QStringList entries = resourceDir.entryList();
    qDebug() << "Resources in :/assets:" << entries;

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::gameLoop() {
    tick();
}

void MainWindow::tick() {
    switch (state) {
    case IDLE:
        break; // do nothing
    case INITGAME:
        print_game_state();
        game.init_new_game();
        qDebug() << "SB has bet $" << SMALLBLIND << "\nBB has bet $" << BIGBLIND << "\n";
        state = STARTROUND;
        print_players_status();
        break;
    case STARTROUND:
        print_round_state();
        state = PLAYERACTION;
        break;
    case PLAYERACTION:
        if (!actionReady) { // Wait for user to press button
            ui->slider_raise->setMinimum(max(game.get_current_player().get_to_call(), BIGBLIND)); // min bet is bigblind (not 0)
            ui->slider_raise->setMaximum(game.get_current_player().get_stack());
            if (ui->slider_raise->minimum() == ui->slider_raise->maximum()) ui->button_raise->setDisabled(true);
            else ui->button_raise->setDisabled(false);
            return;
        }
        game.make_action(pendingAction);
        actionReady = false;
        if (game.betting_over()) {
            state = BETTINGOVER;
        }
        game.debug_state();
        break;
    case BETTINGOVER:
        if (game.game_end()) {
            state = DISTRIBUTEPOT;
        } else {
            state = DRAWCARDS;
        }
        break;
    case DRAWCARDS:
        game.draw_community_cards();
        game.next_round();
        state = STARTROUND;
        break;
    case DISTRIBUTEPOT:
        vector<int> winners = game.compute_winners_and_distribute_pot();
        state = INITGAME;
        break;
    }

    // game number and pot
    ui->display_gameno->setText(QString::number(game.get_gameNo()));
    ui->display_pot->setText("$" + QString::number(game.get_pot()));

    static QLineEdit* display_usernames[] = {
        ui->username_0,
        ui->username_1,
        ui->username_2,
        ui->username_3,
        ui->username_4,
        ui->username_5,
    };
    for (int i = 0; i < int(game.get_players().size()); ++i) {
        display_usernames[i]->setText(QString::fromStdString(game.get_players()[i].get_username()));
    }

    // player stacks
    static QLineEdit* display_stacks[] = {
        ui->display_stack0,
        ui->display_stack1,
        ui->display_stack2,
        ui->display_stack3,
        ui->display_stack4,
        ui->display_stack5,
    };
    for (int i = 0; i < int(game.get_players().size()); ++i) {
        display_stacks[i]->setText("$" + QString::number(game.get_players()[i].get_stack()));
    }

    // Dealer/SB/BB displays
    static QLineEdit* display_role[] = {
                                        ui->role_p0,
                                        ui->role_p1,
                                        ui->role_p2,
                                        ui->role_p3,
                                        ui->role_p4,
                                        ui->role_p5 };
    for (int i = 0; i < int(game.get_players().size()); ++i) {
        if (game.get_players()[i] == game.get_current_dealer()) {
            display_role[i]->setText(QString::fromStdString("D"));
        } else if (game.get_players()[i] == game.get_sb()) {
            display_role[i]->setText(QString::fromStdString("SB"));
        } else if (game.get_players()[i] == game.get_bb()) {
            display_role[i]->setText(QString::fromStdString("BB"));
        } else {
            display_role[i]->clear();
        }
    }

    // glow effect on player to act
    auto* glow = new QGraphicsDropShadowEffect;
    glow->setBlurRadius(30); // Adjust glow softness
    glow->setOffset(0);      // Centered glow
    glow->setColor(QColor(255, 215, 0)); // Gold glow
    for (int i = 0; i < int(game.get_players().size()); ++i) {
        if (game.get_players()[i] == game.get_current_player()) {
            display_role[i]->setGraphicsEffect(glow);
        } else {
            display_role[i]->setGraphicsEffect(nullptr);
        }
    }
    ui->display_action_on->setText(QString::number(game.get_current_player().get_playerID()));

    // Display each player's hole cards
    static QLabel* display_holes[] = {
                                      ui->p0_hole1,
                                      ui->p0_hole2,
                                      ui->p1_hole1,
                                      ui->p1_hole2,
                                      ui->p2_hole1,
                                      ui->p2_hole2,
                                      ui->p3_hole1,
                                      ui->p3_hole2,
                                      ui->p4_hole1,
                                      ui->p4_hole2,
                                      ui->p5_hole1,
                                      ui->p5_hole2 };
    for (QLabel* hole : display_holes) hole->clear();
    for (int i = 0; i < int(game.get_players().size()); ++i) {
        if (!game.get_players()[i].get_hole_cards().empty()) {
            int holeno = 0;
            for (Card card : game.get_players()[i].get_hole_cards()) {
                display_card(card, display_holes[2*i + holeno]);
                holeno = (holeno == 0) ? 1 : 0;
            }
        }
    }

    // display board
    static QLabel* display_board[] = {
                                      ui->board_flop1,
                                      ui->board_flop2,
                                      ui->board_flop3,
                                      ui->board_turn,
                                      ui->board_river};
    for (QLabel* card : display_board) {
        card->clear();
    }
    for (int i = 0; i < int(game.get_board().size()); ++i) {
        display_card(game.get_board()[i], display_board[i]);
    }

    // current player's to call
    ui->display_tocall->setText(QString::number(game.get_current_player().get_to_call()));

    // Action log
    ui->display_action->setText(QString::fromStdString(game.get_history_string()));
    ui->display_action->moveCursor(QTextCursor::End);

    // disable/enable buttons based on game state (e.g. can't check if player must call and vice versa)
    if (game.get_current_player().get_to_call() != 0) {
        ui->button_check->setDisabled(true);
        ui->button_call->setDisabled(false);
    } else {
        ui->button_check->setDisabled(false);
        ui->button_call->setDisabled(true);
    }

    qDebug() << "tick";
}

void MainWindow::display_card(Card card, QLabel* label) {

    QString path = ":/assets/" + QString::fromStdString(card.to_filename());
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qWarning() << "Failed to load card image:" << path;
        label->clear();  // Fallback: clear the label if image load fails
        return;
    }

    label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setScaledContents(true); // Optional: ensures full scaling behavior
}

void MainWindow::on_button_call_clicked() {
    pendingAction = Action(CALL, 0);
    actionReady = true;
}

void MainWindow::on_button_check_clicked() {
    pendingAction = Action(CHECK, 0);
    actionReady = true;
}

void MainWindow::on_button_fold_clicked() {
    pendingAction = Action(FOLD, 0);
    actionReady = true;
}

void MainWindow::on_button_raise_clicked() {
    int amount = ui->slider_raise->value();
    pendingAction = Action(RAISE, amount);
    actionReady = true;
}


// -------------- DEBUGGING FUNCTIONS -----------------------


void MainWindow::print_game_state() {
    qDebug() << "================|GAME " << game.get_gameNo() << "|================\n";
    qDebug() << "Dealer: Player " << game.get_current_dealer().get_playerID() << "\n";
    qDebug() << "SB: Player " << game.get_sb().get_playerID() << "\n";
    qDebug() << "BB: Player " << game.get_bb().get_playerID() << "\n";
    qDebug() << "========================================\n";
}

void MainWindow::print_round_state() {
    unordered_map<Round, string> str_to_enum = {{PREFLOP, "preflop"}, {FLOP, "flop"}, {TURN, "turn"}, {RIVER, "RIVER"}};
    qDebug() << "Round: " << str_to_enum[game.get_round()];
    qDebug() << "Pot: $" << game.get_pot();
    QString board_output = "Board: ";
    for (Card card : game.get_board()) board_output += card.to_string() + " ";
    qDebug().noquote() << board_output;
    qDebug() << "\n";
}

void MainWindow::print_players_status() {

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
*/
