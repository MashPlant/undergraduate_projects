#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
Q_OBJECT

public:
    explicit Dialog(int *xp, int *yp, QWidget *parent = 0);

    ~Dialog();

private:
    Ui::Dialog *ui;
    int *xp, *yp;
};

#endif // DIALOG_H
