//
// Created by mashplant on 18-8-28.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->dial1, &QDial::valueChanged, [this](int value)
    { controller.set_c(value); });
    connect(ui->dial2, &QDial::valueChanged, [this](int value)
    { controller.set_f(value); });
    connect(&controller, &TController::c_changed, [this](int value)
    {
        ui->dial1->setValue(value);
        ui->lcdNumber1->display(QString::number(value));
    });
    connect(&controller, &TController::f_changed, [this](int value)
    {
        ui->dial2->setValue(value);
        ui->lcdNumber2->display(QString::number(value));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}