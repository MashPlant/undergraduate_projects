
//
// Created by mashplant on 18-9-6.
//

#ifndef CHESS_IPDIALOG_HPP
#define CHESS_IPDIALOG_HPP

#include <QDialog>
#include <QIntValidator>
#include <QRegExpValidator>
#include <tuple>

namespace Ui
{
    class IPDialog;
}

class IPDialog : public QDialog
{
Q_OBJECT

public:
    explicit IPDialog(QWidget *parent = 0);

    ~IPDialog();

    std::tuple<QString, int, bool> get_result();

private:
    Ui::IPDialog *ui;
    QString ip, port;
    QRegExpValidator ip_validator;
    QIntValidator port_validator;
};

#endif // CHESS_IPDIALOG_HPP
