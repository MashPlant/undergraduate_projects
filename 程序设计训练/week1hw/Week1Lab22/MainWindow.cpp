//
// Created by mashplant on 18-8-29.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "Dialog.hpp"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionInput, &QAction::triggered, []
    {
        Dialog d;
        auto pr = d.get_result();
        if (pr.second)
            qDebug() << pr.first.y << pr.first.m << pr.first.d;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}