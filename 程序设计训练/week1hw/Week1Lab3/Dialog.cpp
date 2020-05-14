#include "Dialog.h"
#include "ui_Dialog.h"
#include <QDebug>

Dialog::Dialog(int *xp, int *yp, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Dialog),
        xp(xp), yp(yp)
{
    ui->setupUi(this);
    connect(ui->lineEditX, &QLineEdit::textChanged, [this](const QString &text)
    {
        bool ok;
        int tmp = text.toInt(&ok);
        if (ok)
            *(this->xp) = tmp;
    });
    connect(ui->lineEditY, &QLineEdit::textChanged, [this](const QString &text)
    {
        bool ok;
        int tmp = text.toInt(&ok);
        if (ok)
            *(this->yp) = tmp;
    });
}

Dialog::~Dialog()
{
    delete ui;
}
