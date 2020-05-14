
//
// Created by mashplant on 18-9-0.
//

#ifndef __SERVER_HPP
#define __SERVER_HPP

#include <QDialog>
#include <QUdpSocket>
#include <QSet>

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
    QUdpSocket socket;
    QSet<QString> ids;
};

#endif // __SERVER_HPP
