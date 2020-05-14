//
// Created by mashplant on 18-8-29.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QPixmap>
#include <QBitmap>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for (int i = 9; i >= 1; --i)
        icons.emplace_back(QString(":/image/%1.jpeg").arg(i));
    load();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::load);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load()
{
    QPushButton *button = ui->pushButton;
    const QPixmap &icon = icons[cur_index];
    button->setIcon(icon);
    button->setIconSize(icon.size());
    button->setFixedSize(icon.size());
    cur_index = (cur_index + 1) % icons.size();
}
