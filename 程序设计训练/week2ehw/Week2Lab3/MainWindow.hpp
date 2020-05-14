//
// Created by mashplant on 18-9-9.
//

#ifndef WEEK2LAB3_MAINWINDOW_HPP
#define WEEK2LAB3_MAINWINDOW_HPP

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QMutexLocker>
#include <memory>

#define MAX_CONNECTIONS 1024

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

public slots:

    void server_New_Connect();

    QByteArray socket_Read_Data(int sock_idx);

private slots:

    void on_startButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer server;
    QTcpSocket *sockets[MAX_CONNECTIONS];
    int active_count;
};

#endif //WEEK2LAB3_MAINWINDOW_HPP