//
// Created by mashplant on 18-9-3.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QSignalMapper>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auto m = new QSignalMapper(this);
    for (int i=1;i<=7;++i)
    {
        QPushButton *b = findChild<QPushButton *>(QString("button%1").arg(i));
        connect(b, SIGNAL(clicked()),  m, SLOT(map()));
        m->setMapping(b, i);
    }
    connect(m, SIGNAL(mapped(int)), this, SLOT(keyPressed(int)));
    connect(ui->buttonQuit, &QPushButton::clicked, [this]
    {
        close();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressed(int i)
{
    qDebug() << "playing";
    ui->textEdit->setText(ui->textEdit->toPlainText() + QString::number(i));
    player.setMedia(QUrl::fromLocalFile(QString("/home/mashplant/Qt/Week1T3/sound/%1.mp3").arg(i)));
    player.setVolume(50);
    player.play();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (Qt::Key_1 <= key && key <= Qt::Key_7)
        keyPressed(key - Qt::Key_1 + 1);
//    {
//        ui->textEdit->setText(ui->textEdit->toPlainText() + QString::number(i));

//    }
//    if (key )
}
