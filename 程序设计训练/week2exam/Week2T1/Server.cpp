//
// Created by mashplant on 18-9-0.
//

#include "Server.hpp"
#include "ui_Server.h"
#include <QDebug>
#include <QNetworkDatagram>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

Server::Server(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Server)
{
    ui->setupUi(this);
    socket.bind(8080);
    connect(&socket, &QUdpSocket::readyRead, [this]
    {
        QByteArray data = socket.receiveDatagram().data();
        QDataStream in(&data, QIODevice::ReadOnly);
        QString name;
        QString id;
        bool male;
        int grade;
        in >> name >> id >> male >> grade;
        if (ids.contains(id))
            QMessageBox::information(this, " ", "duplicate id");
        else
        {
            QString data = QString("%1,%2,%3,%4\r\n")
                    .arg(name)
                    .arg(id)
                    .arg(male ? "male" : "female")
                    .arg(grade);
            ui->textEdit->append(data);
            ids.insert(id);
            QFile file("data.txt");
            file.open(QFile::Append);
            QTextStream out(&file);
            out <<data;
        }
    });
}

Server::~Server()
{
    delete ui;
}