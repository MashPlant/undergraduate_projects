//
// Created by mashplant on 18-9-0.
//

#include "Client.hpp"
#include "ui_Client.h"
#include <QDebug>
#include <QFileDialog>
#include <QHostAddress>

Client::Client(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Client)
{
    ui->setupUi(this);
    bns[0] = ui->buttonMin, bns[1] = ui->buttonMax, bns[2] = ui->buttonAve, bns[3] = ui->buttonMid;
    lbs[0] = ui->editMin, lbs[1] = ui->editMax, lbs[2] = ui->editAve, lbs[3] = ui->editMid;
    socket.connectToHost(QHostAddress::LocalHost, 2018);
    setWindowTitle("Client");
    connect(ui->buttonSelect, &QPushButton::clicked, [this]
    {
        QString name = QFileDialog::getOpenFileName();
        ui->labelFile->setText("Current File: " + name);
        QFile file(name);
        file.open(QFile::ReadOnly);
        file_content = file.readAll();
    });
    connect(&socket, &QTcpSocket::readyRead, [this]
    {
        char data[12]; // op, res
        socket.read(data, 12);
        int op = *(int *) (data);
        double res = *(double *) (data + 4);
        lbs[op]->setText(QString::number(res));
    });
    for (int i = 0; i < 4; ++i)
    {
        connect(bns[i], &QPushButton::clicked, [this, i]
        {
            ui->editMin->clear();
            int size = 0;
            QByteArray send;
            QDataStream s(&send, QIODevice::WriteOnly);
            s << i << file_content;
            size = send.size();
            socket.write((char *) &size, 4);
            socket.waitForBytesWritten();
            socket.write(send);
            socket.waitForBytesWritten();
        });
    }
}

Client::~Client()
{
    delete ui;
}