//
// Created by mashplant on 18-9-8.
//

#ifndef WEEK2LAB22_CLIENT_HPP
#define WEEK2LAB22_CLIENT_HPP

#include "Server.hpp"
#include <cassert>

struct UserRef
{
    int id;
    QString name;
};

struct Client : QObject
{
Q_OBJECT
public:
    int id;
    QString name;
    QTcpSocket socket;
    bool valid = false; // after sign in / sign up => true
    QVector<UserRef> user_list; // retrieve from network
    QStringListModel model;

    QString message; // `return value` of process_data

    Client(QObject *parent);

    void connect_to_server(const QString &ip, int port = 8080);

    void sign_in(const QString &name, const QString &password);

    void sign_up(const QString &name, const QString &password);

    void send_message(const QString &message, int receiver);

    void populate(QListView &view);

public slots:

    void process_data();

signals:

    void user_list_changed();

    void message_changed();

    void valid_changed(bool valid);
};

#endif //WEEK2LAB22_CLIENT_HPP
