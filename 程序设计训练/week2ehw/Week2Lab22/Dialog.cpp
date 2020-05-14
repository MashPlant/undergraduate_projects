//
// Created by mashplant on 18-9-8.
//

#include "Dialog.hpp"
#include "ui_Dialog.h"
#include "Client.hpp"
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Dialog),
        client(new Client(this))
{
    ui->setupUi(this);
    ui->editName->setVisible(false);
    ui->editPassword->setVisible(false);
    ui->buttonSignIn->setVisible(false);
    ui->buttonSignUp->setVisible(false);
    connect(ui->buttonConnect, &QPushButton::clicked, [this]
    {
        client->connect_to_server(ui->editIp->text());
        ui->editIp->setVisible(false);
        ui->buttonConnect->setVisible(false);
        ui->editName->setVisible(true);
        ui->editPassword->setVisible(true);
        ui->buttonSignIn->setVisible(true);
        ui->buttonSignUp->setVisible(true);
        update();
    });
    connect(ui->buttonSignIn, &QPushButton::clicked,
            [this] { client->sign_in(ui->editName->text(), ui->editPassword->text()); });
    connect(ui->buttonSignUp, &QPushButton::clicked,
            [this] { client->sign_up(ui->editName->text(), ui->editPassword->text()); });
    connect(client, &Client::valid_changed, [this](bool valid)
    {
        QMessageBox *info = new QMessageBox(this);
        info->setWindowTitle(" ");
        info->setText(client->message);
        info->show();
        if (valid)
        {
            setWindowTitle(client->name);
            ui->editName->setVisible(false);
            ui->editPassword->setVisible(false);
            ui->buttonSignIn->setVisible(false);
            ui->buttonSignUp->setVisible(false);
        }
    });
    connect(client, &Client::user_list_changed, this, &Dialog::update);
    connect(client, &Client::message_changed, [this] { ui->editDisplay->append(client->message); });
    connect(ui->buttonSend, &QPushButton::clicked, [this]
    {
        auto index = ui->listView->currentIndex();
        if (ui->listView->currentIndex().isValid())
            client->send_message(ui->editInput->toPlainText(), index.row());
    });
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::update()
{
    client->populate(*ui->listView);
    QDialog::update();
}
