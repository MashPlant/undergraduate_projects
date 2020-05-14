
//
// Created by mashplant on 18-9-0.
//

#ifndef __CLIENT_HPP
#define __CLIENT_HPP

#include <QDialog>
#include <QTcpSocket>

struct QPushButton;
struct QLineEdit;

namespace Ui
{
    class Client;
}

class Client : public QDialog
{
Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);

    ~Client();

private:
    Ui::Client *ui;
    QTcpSocket socket;
    QByteArray file_content;
    QPushButton *bns[4];
    QLineEdit *lbs[4];
};

#endif // __CLIENT_HPP
