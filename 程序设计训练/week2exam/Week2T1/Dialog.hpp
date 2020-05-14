
//
// Created by mashplant on 18-9-0.
//

#ifndef __DIALOG_HPP
#define __DIALOG_HPP

#include <QDialog>
#include <QUdpSocket>

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

    QString name;
    QString id;
    bool male;
    int grade;
private:
    Ui::Dialog *ui;
    QUdpSocket socket;

};

#endif // __DIALOG_HPP
