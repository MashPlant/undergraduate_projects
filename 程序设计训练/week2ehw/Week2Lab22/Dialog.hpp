
//
// Created by mashplant on 18-9-8.
//

#ifndef WEEK2LAB22_DIALOG_HPP
#define WEEK2LAB22_DIALOG_HPP

#include <QDialog>
#include <QTcpSocket>

struct Client;

namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);

    ~Dialog();

    void update();

private:
    Ui::Dialog *ui;
    Client *client;
};

#endif // WEEK2LAB22_DIALOG_HPP
