//
// Created by mashplant on 18-9-8.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "Dialog.hpp"
#include "Server.hpp"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        server(new Server(this))
{
    ui->setupUi(this);
    connect(ui->actionClient, &QAction::triggered, [this]
    {
        Dialog *d = new Dialog(this);
        d->show();
        connect(d, &QDialog::finished, d, &QDialog::deleteLater);
    });
    connect(server, &Server::user_list_changed, this, &MainWindow::update);
}

MainWindow::~MainWindow()
{
    server->write_to_file();
    delete ui;
}

void MainWindow::update()
{
    server->populate(*ui->listView);
    QMainWindow::update();
}
