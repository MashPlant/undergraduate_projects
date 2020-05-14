//
// Created by mashplant on 18-9-0.
//

#include "Dialog.hpp"
#include "ui_Dialog.h"
#include <QDataStream>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &Dialog::reject);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, [this]
    {
        /*客户端进行初步的合法性检查:姓名不超过 10 个字符;学
        号为 10 位数字;性别为单选组合框,用户选择男、女;成绩在 0-100 之间*/
        name = ui->name->text();
        id = ui->id->text();
        male = ui->gender->currentIndex() == 0;
        grade = ui->grade->text().toInt();
        if (name.size() > 10)
            return void(QMessageBox::information(this, " ", "invalid name"));
        if (id.size() != 10)
            return void(QMessageBox::information(this, " ", "invalid id"));
        for (int i = 0; i < 10; ++i)
            if (id[i] > '9' || id[i] < '0')
                return void(QMessageBox::information(this, " ", "invalid id"));
        if (grade < 0 || grade > 100)
            return void(QMessageBox::information(this, " ", "invalid grade"));
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << name << id << male << grade;
        socket.writeDatagram(data, QHostAddress::LocalHost, 8080);
    });
}

Dialog::~Dialog()
{
    delete ui;
}