//
// Created by mashplant on 18-9-3.
//

#ifndef CHESS_MAINWINDOW_HPP
#define CHESS_MAINWINDOW_HPP

#include "ChessFrame.hpp"
#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMessageBox>
#include <QTimer>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(bool is_server, QWidget *parent = 0);

    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    const static int timeout_min = 0, timeout_sec = 20;

    Ui::MainWindow *ui;
    ChessFrame frame;
    QTcpSocket *socket;
    QTcpServer *server;
    QMessageBox *wait_response;
    QMessageBox *wait_connect;
    bool is_server;
    int player;
    int want_new_game;
    QString load_file_name;
    int second_count;
    QTimer second;

    void update();

    // if from_tcp == true, then will not send the message
    void exec_move(int f, int t, bool from_tcp);

    // info comes from exec_move
    void handle_winner_info(uint32_t info);

    void query_new_game(const QString &msg);

    // load & save game without network
    void _load_game();

    void _save_game();

    void start(bool reset = false);

    enum
    {
        ExecMove,
        NewGameRequest,
        NewGameResponse,
        SaveGame,
        LoadGameRequest,
        LoadGameResponse,
        Submit,
        Timeout,
    };
private slots:

    void process_data();

    void save_game();

    void load_game();

signals:

    void load_game_response(bool agree);

    void new_game_response(bool agree);
};

#endif //CHESS_MAINWINDOW_HPP
