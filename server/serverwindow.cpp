#include "serverwindow.hpp"
#include "ui_serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWindow)
    , server(new Server(this))
{
    ui->setupUi(this);
    connect(ui->startStopButton, &QPushButton::clicked, this, &ServerWindow::toggleStartServer);
    connect(server, &Server::logMessage, this, &ServerWindow::logMessage);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::toggleStartServer()
{
    if (server->isListening()) {
        server->stopServer();
        ui->startStopButton->setText(tr("Start Server"));
        logMessage(QStringLiteral("Server Stopped"));
    } else {
        if (!server->listen(QHostAddress::Any, SERVER_PORT)) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to start the server"));
            return;
        }
        logMessage(QStringLiteral("Server Started"));
        ui->startStopButton->setText(tr("Stop Server"));
    }
}

void ServerWindow::logMessage(const QString &msg)
{
    ui->logEditor->appendPlainText(msg + QLatin1Char('\n'));
}
