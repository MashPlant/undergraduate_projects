//
// Created by mashplant on 18-9-8.
//

#include "Server.hpp"
#include <QFile>

Server::Server(QObject *parent)
        : QObject(parent)
{
    server.listen(QHostAddress::Any, 8080);
    connect(&server, &QTcpServer::newConnection, [this]
    {
        QTcpSocket *socket = server.nextPendingConnection();
        qDebug() << "new connection";
        connect(socket, &QTcpSocket::disconnected, [this, socket]
        {
            qDebug() << "connection disconnected";
            QMutexLocker locker(&mutex);
            for (int i = 0; i < user_list.size(); ++i)
                if (user_list[i].socket == socket)
                {
                    user_list.remove(i);
                    publish_user_list();
                    emit user_list_changed();
                }
        });
        connect(socket, &QTcpSocket::readyRead, [this, socket] { process_data(socket); });
        QMutexLocker locker(&mutex);
        user_list.push_back({-1, "", socket});
        emit user_list_changed();
    });
    QFile file("all_users");
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_9);
        in >> all_users;
    }
}

void Server::populate(QListView &view)
{
    QStringList display_list;
    for (auto &user : user_list)
        display_list += QString("id = %1; name = %2; address = %3")
                .arg(user.id)
                .arg(user.name)
                .arg(user.socket->localAddress().toString());
    model.setStringList(display_list);
    view.setModel(&model);
}

void Server::process_data(QTcpSocket *socket)
{
    QByteArray data = socket->readAll();
    const char *data_ptr = data.data() + 1;
    int data_len = data.size() - 1;
    QMutexLocker locker(&mutex);
    switch (data[0])
    {
        case Connect: // expect to get this kind of message instantly after connected
        {
            int sender = read_int(data_ptr);
            for (int i = 0; i < user_list.size(); ++i)
                if (user_list[i].socket == socket)
                    user_list[i].id = sender;
        }
            break;
        case SignIn:
        {
            auto result = QString(data.mid(1)).split(SplitChar);
            QString name = result[0], password = result[1];
            if (!all_users.contains(name))
                write_chars(socket, SignIn, NoSuchUser);
            else if (all_users.value(name) != password)
                write_chars(socket, SignIn, WrongPassword);
            else
            {
                for (int i = 0; i < user_list.size(); ++i)
                    if (user_list[i].name == name)
                        return write_chars(socket, SignIn, AlreadyOnline);
                for (int i = 0; i < user_list.size(); ++i)
                    if (user_list[i].socket == socket)
                        user_list[i].name = name;
                emit user_list_changed();
                write_chars(socket, SignIn, Ok);
                socket->waitForBytesWritten(); // prevent 2 consecutive message being sent together
                publish_user_list();
            }
        }
            break;
        case SignUp:
        {
            auto result = QString(data.mid(1)).split(SplitChar);
            QString name = result[0], password = result[1];
            if (all_users.contains(name))
                return void(write_chars(socket, SignUp, DuplicateName));
            all_users[name] = password;
            for (int i = 0; i < user_list.size(); ++i)
                if (user_list[i].socket == socket)
                {
                    user_list[i].name = name;
                    emit user_list_changed();
                    write_chars(socket, SignUp, Ok);
                    socket->waitForBytesWritten();
                    publish_user_list();
                }
        }
            break;
        case Message:
        {
            int sender = read_int(data_ptr);
            int receiver = read_int(data_ptr + 4);
            for (int i = 0; i < user_list.size(); ++i)
                if (user_list[i].id == receiver)
                {
                    QByteArray data(5, '\0');
                    data[0] = Message;
                    write_int(data.data() + 1, sender);
                    data += QByteArray(data_ptr + 8, data_len - 8);
                    user_list[i].socket->write(data);
                }
        }
            break;
        default:
            qDebug() << "server unknown type" << int(data[0]);
            break;
    }
}


void Server::publish_user_list()
{
    QByteArray data(1, GetList);
    for (int i = 0; i < user_list.size(); ++i)
        if (!user_list[i].name.isEmpty())
            data += QString::number(user_list[i].id) + SplitChar + user_list[i].name + SplitChar;
    if (data.size() > 1)
        data.remove(data.size() - 1, 1);
    for (int i = 0; i < user_list.size(); ++i)
        user_list[i].socket->write(data);
}

void Server::write_to_file()
{
    QFile file("all_users");
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_3);
        out << all_users;
    }
}
