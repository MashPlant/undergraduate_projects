//
// Created by mashplant on 18-9-8.
//

#include "Client.hpp"

Client::Client(QObject *parent) : QObject(parent)
{
    static int id = 0;
    this->id = ++id;
};

void Client::connect_to_server(const QString &ip, int port /* = 8080*/)
{
    socket.connectToHost(ip, port);
    connect(&socket, &QTcpSocket::readyRead, this, &Client::process_data);
    socket.waitForConnected();
    char data[5] = {Connect};
    write_int(data + 1, id);
    socket.write(data, 5);
    socket.waitForBytesWritten();
}

void Client::sign_in(const QString &name, const QString &password)
{
    if (name.isEmpty() || password.isEmpty())
    {
        message = "Name and password shouldn't be empty.";
        emit valid_changed(false);
        return;
    }
    this->name = name;
    socket.write((char(SignIn) + name + SplitChar + password).toLocal8Bit());
}

void Client::sign_up(const QString &name, const QString &password)
{
    if (name.isEmpty() || password.isEmpty())
    {
        message = "Name and password shouldn't be empty.";
        emit valid_changed(false);
        return;
    }
    this->name = name;
    socket.write((char(SignUp) + name + SplitChar + password).toLocal8Bit());
}

void Client::send_message(const QString &message, int receiver)
{
    if (!valid)
        return;
    QByteArray data(9, '\0');
    data[0] = Message;
    write_int(data.data() + 1, id);
    write_int(data.data() + 5, user_list[receiver].id);
    data += message;
    socket.write(data);
}

void Client::populate(QListView &view)
{
    QStringList content;
    for (int i = 0; i < user_list.size(); ++i)
        content += user_list[i].name;
    model.setStringList(content);
    view.setModel(&model);
}

const QString status_to_string[] =
        {
                [Ok] = "Success.",
                [NoSuchUser] = "No such user.",
                [WrongPassword] = "Wrong password.",
                [DuplicateName] = "User already exists.",
                [AlreadyOnline] = "User already online."
        };

void Client::process_data()
{
    QByteArray data = socket.readAll();
    const char *data_ptr = data.data() + 1;
    int data_len = data.size() - 1;

    switch (data[0])
    {
        case SignIn:
        case SignUp:
            valid = data[1] == char(Ok);
            message = status_to_string[data[1]];
            emit valid_changed(valid);
            break;
        case Message:
        {
            int sender = read_int(data_ptr);
            QString sender_name;
            for (int i = 0; i < user_list.size(); ++i)
                if (user_list[i].id == id)
                    sender_name = user_list[i].name;
            message = sender_name + " says: " + QString(data.mid(5));
            emit message_changed();
        }
            break;
        case GetList:
        {
            QList<QByteArray> split_data = data.mid(1).split(SplitChar);
            user_list.clear();
            for (int i = 0; i * 2 < split_data.size(); ++i)
            {
                int id = split_data[i * 2].toInt();
                QString name = split_data[i * 2 + 1];
                user_list.append({id, name});
                emit user_list_changed();
            }
        }
            break;
        default:
            qDebug() << "server unknown type" << int(data[0]);
            break;
    }
}
