//
// Created by mashplant on 18-9-3.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "IPDialog.hpp"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QResizeEvent>
#include <QTime>
#include <QSound>
#include <QMediaPlayer>

bool timer_active;
#define TIMER_PAUSE(t) do { timer_active = t.isActive(), t.stop(); } while(0)
#define TIMER_RESTART(t) do { if (timer_active) t.start(); } while(0)

MainWindow::MainWindow(bool is_server, QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        is_server(is_server),
        player(0),
        want_new_game(0),
        second_count(0)
{
    ui->setupUi(this);
    if (is_server)
    {
        setWindowTitle("Server");
        ui->actionConnect->setText("&Listen");
        server = new QTcpServer(this), socket = nullptr;
        wait_connect = new QMessageBox(this);
        wait_connect->setWindowTitle("Please wait");
        wait_connect->setText("Waiting for connection...");
        connect(ui->actionConnect, &QAction::triggered, [this]
        {
            server->listen(QHostAddress::Any, 8080);
            wait_connect->show();
        });
        connect(wait_connect, &QMessageBox::finished, server, &QTcpServer::close); 
        connect(server, &QTcpServer::newConnection, [this]
        {
            socket = server->nextPendingConnection();
            connect(socket, &QTcpSocket::readyRead, this, &MainWindow::process_data);
            QMessageBox::information(this, "Connection success!",
                                     QString("Connected to %1").arg(socket->peerAddress().toString()));
            wait_connect->close();
            start(true);
        });
    }
    else
    {
        setWindowTitle("Client");
        server = nullptr, socket = new QTcpSocket(this);
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::process_data);
        connect(ui->actionConnect, &QAction::triggered, [this]
        {
            IPDialog d;
            QString server_ip;
            int port;
            bool ok;
            std::tie(server_ip, port, ok) = d.get_result();
            if (ok)
            {
                socket->connectToHost(server_ip, port);
                connect(socket, &QTcpSocket::connected, [this, server_ip]
                {
                    QMessageBox::information(this, "Connection success!", QString("Connected to %1").arg(server_ip));
                    start(true);
                });
            }

        });
    }
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save_game);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::load_game);

    wait_response = new QMessageBox(this);
    wait_response->setWindowTitle("Please wait");
    wait_response->setText("Waiting for your opponent's permission...");
    // you cannot close it
    // {
    //     wait_response->setStandardButtons(QMessageBox::NoButton);
    //     wait_response->setWindowFlags(wait_response->windowFlags() & ~Qt::WindowCloseButtonHint);
    // }
    connect(this, &MainWindow::load_game_response, wait_response, &QMessageBox::hide);
    connect(this, &MainWindow::new_game_response, wait_response, &QMessageBox::hide);

    connect(ui->buttonSubmit, &QPushButton::clicked, [this]
    {
        if (player)
        {
            TIMER_PAUSE(second);
            if (QMessageBox::question(this, "Submit", "Are you sure to submit?") == QMessageBox::Yes)
            {
                char data = Submit;
                socket->write(&data, 1);
                query_new_game(QString("%1 submitted. New game?").arg(player == ChessFrame::black ? "Black" : "Red"));
            }
            TIMER_RESTART(second);
        }
    });
    connect(&second, &QTimer::timeout, [this]
    {
        const int all = timeout_min * 60 + timeout_sec;
        if (++second_count == all)
        {
            char data = Timeout;
            socket->write(&data, 1);
            query_new_game(QString("%1 timeout. New game?").arg(player == ChessFrame::black ? "Black" : "Red"));
        }
        update();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::start(bool reset /* = false */)
{
    if (reset)
        want_new_game = 0, frame = {};
    second_count = 0;
    player = is_server ? ChessFrame::red : ChessFrame::black;
    if (player == frame.player)
        second.start(1000);
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    const double factor = (std::min(width(), height()) * 0.8 - 20) / ChessFrame::Y;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(width() / 2, height() / 2);
    painter.scale(factor, factor);
    painter.setPen(QPen(Qt::black, 1 / factor));
    painter.setBrush(Qt::blue);
    frame.paint(painter);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (player)
    {
        const double factor = (std::min(width(), height()) * 0.8 - 20) / ChessFrame::Y;
        uint32_t maybe_move = frame.click((event->x() - width() / 2) / factor,
                                          (event->y() - height() / 2) / factor, player);
        if (maybe_move != -1u)
            exec_move(maybe_move >> 16, maybe_move & 65535, false);
        update();
    }
}

void MainWindow::process_data()
{
    QByteArray data = socket->readAll();
    switch (data[0])
    {
        case ExecMove:
            exec_move(data[1], data[2], true);
            break;
        case NewGameRequest:
            if (++want_new_game == 2)
            {
                char data = NewGameResponse;
                socket->write(&data, 1);
                emit new_game_response(true);
                start(true);
            }
            break;
        case NewGameResponse:
            qDebug() << data;
            emit new_game_response(true);
            start(true);
            break;
        case SaveGame:
            TIMER_PAUSE(second);
            if (QMessageBox::question(this, "Your opponent want to save game", "Do you accept?") == QMessageBox::Yes)
                _save_game();
            TIMER_RESTART(second);
            break;
        case LoadGameRequest:
        {
            TIMER_PAUSE(second);
            bool res = QMessageBox::question(this, "Your opponent want to load game", "Do you accept?") ==
                       QMessageBox::Yes;
            TIMER_RESTART(second);
            {
                char data[2] = {LoadGameResponse, res};
                socket->write(data, 2);
            }
            if (res)
            {
                QTextStream stream(data.mid(1));
                stream >> frame;
                start();
            }
        }
            break;
        case LoadGameResponse: // response load game
            emit load_game_response(data[1]);
            if (data[1])
            {
                TIMER_PAUSE(second);
                _load_game();
                TIMER_RESTART(second);
            }
            break;
        case Submit:
            query_new_game(QString("%1 submitted. New game?").arg(player == ChessFrame::black ? "Red" : "Black"));
            break;
        case Timeout:
            query_new_game(QString("%1 timeout. New game?").arg(player == ChessFrame::black ? "Red" : "Black"));
            break;
        default:
            qDebug() << "unknown type" << int(data[0]);
            break;
    }
}

void MainWindow::update()
{
    if (!player)
        ui->statusBar->showMessage("Not started yet.");
    else
        ui->statusBar->showMessage(QString("Your color: %1, now %2 turn.")
                                           .arg(player == ChessFrame::red ? "red" : "black")
                                           .arg(player == frame.player ? "your" : "opponent's"));
    const int all = timeout_min * 60 + timeout_sec;
    QTime time(0, (all - second_count) / 60, (all - second_count) % 60);
    ui->lcdNumber->display(time.toString("mm:ss"));
    QWidget::update();
}

void MainWindow::exec_move(int f, int t, bool from_tcp)
{
    second_count = 0;
    if (player == frame.player)
        second.stop();
    else
        second.start(1000);
    uint32_t info = frame.exec_move(f, t);
    update();
    if (!from_tcp && socket)
    {
        char data[3] = {ExecMove, char((info >> 8) & 255), char(info & 255)};
        socket->write(data, 3);
    }
    handle_winner_info(info);
}

void MainWindow::handle_winner_info(uint32_t info)
{
    uint32_t maybe_winner = (info >> 16) & 255;
    if (maybe_winner)
        query_new_game(QString("%1 wins. New Game?").arg(maybe_winner == ChessFrame::black ? "Black" : "Red"));
}

void MainWindow::query_new_game(const QString &msg)
{
    second.stop();
    int res = QMessageBox::question(this, "Game over", msg);
    if (res == QMessageBox::Yes)
    {
        if (++want_new_game == 2)
        {
            char data = NewGameResponse;
            socket->write(&data, 1);
            start(true);
        }
        else
        {
            char data = NewGameRequest;
            socket->write(&data, 1);
            wait_response->show();
        }
    }
    else
        close();
}

void MainWindow::_save_game()
{
    QString file_name = QFileDialog::getSaveFileName(this, "Save to");
    QFile file(file_name);
    if (file.open(QFile::WriteOnly))
    {
        QTextStream stream(&file);
        stream << frame;
    }
}

void MainWindow::save_game()
{
    TIMER_PAUSE(second);
    if (socket && socket->isWritable())
    {
        char data = SaveGame;
        socket->write(&data, 1);
    }
    _save_game();
    TIMER_RESTART(second);
}

void MainWindow::_load_game()
{
    QFile file(load_file_name);
    if (file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);
        stream >> frame;
        start();
    }
}

void MainWindow::load_game()
{
    TIMER_PAUSE(second);
    if (socket && socket->isWritable())
    {
        load_file_name = QFileDialog::getOpenFileName(this, "Load from");
        QFile file(load_file_name);
        if (file.open(QFile::ReadOnly))
        {
            socket->write(char(LoadGameRequest) + file.readAll());
            wait_response->exec();
        }
    }
    TIMER_RESTART(second);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    int w = width(), h = height();
    int widget_w = w / 5, widget_h = h / 20;
    ui->lcdNumber->setGeometry(w / 2 - widget_w / 2, h - widget_h * 4, widget_w, widget_h);
    ui->buttonSubmit->setGeometry(w / 2 - widget_w / 2, h - widget_h * 3, widget_w, widget_h);
}
