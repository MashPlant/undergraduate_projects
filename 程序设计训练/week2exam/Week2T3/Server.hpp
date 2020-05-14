
//
// Created by mashplant on 18-9-0.
//

#ifndef __SERVER_HPP
#define __SERVER_HPP

#include <QDialog>
#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>

namespace Ui
{
    class Server;
}

class Server : public QDialog
{
Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);

    ~Server();

private:
    Ui::Server *ui;
    QTcpServer server;
    QVector<QTcpSocket *> clients;
    QVector<int> file_size;
};

#endif // __SERVER_HPP
