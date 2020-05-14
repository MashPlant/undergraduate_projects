#include "Dialog.hpp"
#include "ui_Dialog.h"
#include <QDebug>
#include <QPushButton>
#include <QDialogButtonBox>

Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->buttonForward, &QRadioButton::clicked, [this]
    { result.backward = false; });
    connect(ui->buttonBackward, &QRadioButton::clicked, [this]
    { result.backward = true; });
    connect(this, &Dialog::accepted, [this]
    { result.content = ui->lineEdit->text(); });
}

Dialog::~Dialog()
{
    delete ui;
}
