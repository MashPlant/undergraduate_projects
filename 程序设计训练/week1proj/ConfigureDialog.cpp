//
// Created by mashplant on 18-8-27.
//

#include "ConfigureDialog.hpp"
#include "ui_ConfigureDialog.h"
#include <QDebug>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QKeyEvent>

ConfigureDialog::ConfigureDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ConfigureDialog)
{
    ui->setupUi(this);
    ui->labelResponse->setStyleSheet("color:red");
    ui->buttonOk->setVisible(false);
    for (int i = 1; i <= 6; ++i)
    {
        auto edit = findChild<QLineEdit *>(QString("lineEdit%1").arg(i));
        connect(edit, &QLineEdit::textChanged, [this]
        {
            bool ok = _get_configure().second;
            ui->buttonOk->setVisible(ok);
            ui->labelResponse->setVisible(!ok);
        });
    }
    connect(ui->buttonOk, &QPushButton::clicked, [this]
    {
        configure = _get_configure();
        accept();
    });
}

ConfigureDialog::~ConfigureDialog()
{
    delete ui;
}

std::pair<ConfigureInfo, bool> ConfigureDialog::_get_configure()
{
    bool ok = true;
    auto to_int = [&ok](const QLineEdit *edit)
    { 
        bool ok_ = true;
        int ret = edit->text().toInt(&ok_);
        ok &= ok_;
        return ret;
    };
    int count = to_int(ui->lineEdit1);
    int in1 = to_int(ui->lineEdit2),
            in2 = to_int(ui->lineEdit3);
    int out1 = to_int(ui->lineEdit4),
            out2 = to_int(ui->lineEdit5),
            out3 = to_int(ui->lineEdit6);
    if (ok &&
        5 <= count && count <= 8 &&
        0 <= in1 && in1 < count && 0 <= in2 && in2 < count &&
        in1 < in2 &&
        0 <= out1 && out1 < count && 0 <= out2 && out2 < count && 0 <= out3 && out3 < count
        && out1 < out2 && out2 < out3)
        return {{count, in1, in2, out1, out2, out3}, true};
    return {{}, false};
}

std::pair<ConfigureInfo, bool> ConfigureDialog::get_configure()
{
    if (exec() == Accepted)
        return configure;
    return {{}, false};
}

void ConfigureDialog::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Return && ui->buttonOk->isVisible())
        ui->buttonOk->click();
}
