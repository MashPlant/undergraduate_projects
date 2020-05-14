//
// Created by mashplant on 18-9-8.
//

#ifndef WEEK2LAB22_SERVER_HPP
#define WEEK2LAB22_SERVER_HPP

#include <QMutex>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QListView>
#include <QStringListModel>
#include <QHash>

struct User
{
    int id;
    QString name;
    QTcpSocket *socket;
};

enum
{
    Connect,
    SignIn,
    SignUp,
    Message,
    GetList,
};

enum
{
    Ok,
    NoSuchUser,
    WrongPassword,
    DuplicateName,
    AlreadyOnline,
};

enum
{
    SplitChar = '$',
};

inline int read_int(const char *s)
{
    int x;
    memcpy(&x, s, sizeof(int));
    return x;
}

inline void write_int(char *s, int x)
{ memcpy(s, &x, sizeof(int)); }

inline void write_chars(QTcpSocket *socket, char a)
{ socket->write(&a, 1); }

inline void write_chars(QTcpSocket *socket, char a, char b)
{
    char data[] = {a, b};
    socket->write(data, 2);
}

inline void write_chars(QTcpSocket *socket, char a, char b, char c)
{
    char data[] = {a, b, c};
    socket->write(data, 3);
}

struct Server : QObject
{
Q_OBJECT
public:
    QVector<User> user_list;
    mutable QMutex mutex;
    QTcpServer server;
    QStringListModel model;
    QHash<QString, QString> all_users;

    Server(QObject *parent = nullptr);

    void populate(QListView &view);

    void process_data(QTcpSocket *sender);

    void publish_user_list();

    void write_to_file();

signals:

    void user_list_changed();
};

#endif //WEEK2LAB22_SERVER_HPP
