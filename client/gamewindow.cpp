#include "gamewindow.hpp"
#include "ui_gamewindow.h"

#include <QmessageBox>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWindow)
    , clientPlaying(new Client(this))
{
    ui->setupUi(this);
    connect(clientPlaying, &Client::connected, this, &GameWindow::connectedToServer);
    connect(clientPlaying, &Client::loggedIn, this, &GameWindow::loggedIn);
    connect(clientPlaying, &Client::loginError, this, &GameWindow::loginFailed);
    connect(clientPlaying, &Client::actionReceived, this, &GameWindow::actionReceived);
    connect(clientPlaying, &Client::disconnected, this, &GameWindow::disconnectedFromServer);
    connect(clientPlaying, &Client::error, this, &GameWindow::error);
    connect(clientPlaying, &Client::newClientJoined, this, &GameWindow::newClientJoined);
    connect(clientPlaying, &Client::clientLeft, this, &GameWindow::clientLeft);
    connect(ui->button_call, &QPushButton::clicked, this, &GameWindow::call);
    connect(ui->button_check, &QPushButton::clicked, this, &GameWindow::check);
    connect(ui->button_fold, &QPushButton::clicked, this, &GameWindow::fold);
    connect(ui->button_raise, &QPushButton::clicked, this, &GameWindow::raise);

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

void GameWindow::loggedIn() {
    // nothing needs to be done, just send logged in signal
}

void GameWindow::loginFailed(const QString &reason) {
    QMessageBox::critical(this, tr("Error"), reason);
    emit returnToLogin();
}

void GameWindow::returnToLogin() {

}

void GameWindow::disconnectedFromServer() {
    QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));
    emit returnToLogin();
}

void GameWindow::newClientJoined(const QString &username, int player_id) {
    // TODO
}

void GameWindow::clientLeft(const QString &username, int player_id) {
    // TODO
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

void GameWindow::call() {

}
void GameWindow::check() {

}
void GameWindow::fold() {

}
void GameWindow::raise() {

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
