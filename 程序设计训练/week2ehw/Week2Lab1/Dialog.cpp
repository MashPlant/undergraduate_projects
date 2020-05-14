//
// Created by mashplant on 18-9-5.
//

#include "Dialog.hpp"
#include "ui_Dialog.h"
#include <QDebug>
#include <QNetworkDatagram>

Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Dialog)
{
    static int count = 0;
    id = count++;
    ui->setupUi(this);
    socket.bind(8888 ^ id);
    connect(&socket, &QUdpSocket::readyRead, [this]
    {
        qDebug() << id << "receive data";
        while (socket.hasPendingDatagrams())
            ui->display->setText(ui->display->toPlainText() + socket.receiveDatagram().data());
    });
    connect(ui->send, &QPushButton::clicked, [this]
    {
        qDebug() << id << "send data";
        socket.writeDatagram(ui->edit->toPlainText().toLocal8Bit(), QHostAddress::LocalHost, 8888 ^ (id ^ 1));
    });
}

Dialog::~Dialog()
{
    delete ui;
}