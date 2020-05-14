
//
// Created by mashplant on 18-9-5.
//

#ifndef WEEK2LAB1_DIALOG_HPP
#define WEEK2LAB1_DIALOG_HPP

#include <QDialog>
#include <QUdpSocket>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
    QUdpSocket socket;
    int id;
};

#endif // WEEK2LAB1_DIALOG_HPP
