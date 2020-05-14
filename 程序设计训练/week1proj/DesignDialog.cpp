//
// Created by mashplant on 18-8-30.
//

#include "DesignDialog.hpp"
#include "ui_DesignDialog.h"
#include <QDebug>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QKeyEvent>

DesignDialog::DesignDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DesignDialog)
{
    ui->setupUi(this);
    ui->labelResponse->setStyleSheet("color:red");
    ui->buttonOk->setVisible(false);
    for (int i = 1; i <= 4; ++i)
    {
        auto edit = findChild<QLineEdit *>(QString("lineEdit%1").arg(i));
        connect(edit, &QLineEdit::textChanged, [this]
        {
            bool ok = _get_design().second;
            ui->buttonOk->setVisible(ok);
            ui->labelResponse->setVisible(!ok);
        });
    }
    connect(ui->buttonOk, &QPushButton::clicked, [this]
    {
        design = _get_design();
        accept();
    });
}

DesignDialog::~DesignDialog()
{
    delete ui;
}

std::pair<DesignInfo, bool> DesignDialog::_get_design()
{
    bool ok = true;
    auto to_int = [&ok](const QLineEdit *edit)
    {
        bool ok_ = true;
        int ret = edit->text().toInt(&ok_);
        ok &= ok_;
        return ret;
    };
    auto to_double = [&ok](const QLineEdit *edit)
    {
        bool ok_ = true;
        int ret = edit->text().toDouble(&ok_);
        ok &= ok_;
        return ret;
    };
    double exp1 = to_double(ui->lineEdit1),
            exp2 = to_double(ui->lineEdit2),
            exp3 = to_double(ui->lineEdit3);
    int sa_times = to_int(ui->lineEdit4);
    if (ok &&
        0 < exp1 && 0 < exp2 && 0 < exp3 &&
        exp1 + exp2 + exp3 == 400.0 &&
        0 < sa_times)
        return {{exp1, exp2, exp3, sa_times}, true};
    return {{}, false};
}

std::pair<DesignInfo, bool> DesignDialog::get_design()
{
    if (exec() == Accepted)
        return design;
    return {{}, false};
}

void DesignDialog::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Return && ui->buttonOk->isVisible())
        ui->buttonOk->click();
}