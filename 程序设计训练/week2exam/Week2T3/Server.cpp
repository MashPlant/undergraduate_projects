//
// Created by mashplant on 18-9-0.
//

#include "Server.hpp"
#include "ui_Server.h"
#include <QDebug>
#include <vector>
#include <QThread>
#include <algorithm>

template<typename R>
struct Worker : QThread
{
public:
    std::function<R(void)> work;
    R r;

    template<typename F, typename ...Args>
    Worker(QObject *p, F f, Args &&...args):
            QThread(p), work(std::bind(f, std::forward<Args>(args)...))
    {}

    R get() const
    { return r; }

protected:
    void run() override
    {
        r = work();
    }
};

double work(QByteArray file, int op)
{
    double ans = 0.0;
    std::vector<int> data;
    QTextStream in(&file, QIODevice::ReadOnly);
    while (in.status() == QTextStream::Ok)
    {
        int tmp;
        in >> tmp;
        data.push_back(tmp);
    }
    qDebug() << data.size();
    switch (op)
    {
        case 0:
            ans = *std::min_element(data.begin(), data.end());
            break;
        case 1:
            ans = *std::max_element(data.begin(), data.end());
            break;
        case 2:
            ans = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
            break;
        case 3:
            std::nth_element(data.begin(), data.begin() + data.size() / 2, data.end());
            ans = data[data.size() / 2];
            break;
    }
    return ans;
}

Server::Server(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Server)
{
    ui->setupUi(this);
    setWindowTitle("Server");
    server.listen(QHostAddress::Any, 2018);
    connect(&server, &QTcpServer::newConnection, [this]
    {
        QTcpSocket *socket = server.nextPendingConnection();
        clients.push_back(socket);
        file_size.push_back(-1);
        int index = file_size.size() - 1;
        connect(socket, &QTcpSocket::readyRead, [this, socket, index]
        {
            if (file_size[index] == -1) // get op here
            {
                socket->read((char *) (&file_size[index]), 4);
            }
            else
            {
                if (socket->bytesAvailable() < file_size[index])
                    return;
                file_size[index] = -1;
                qDebug() << socket->bytesAvailable() << file_size[index];
                QByteArray data = socket->readAll();
                QDataStream in(&data, QIODevice::ReadOnly);
                int op;
                QByteArray file;
                in >> op >> file;
                qDebug() << file;
                auto w = new Worker<double>(this, work, data, op);
                w->start();
                connect(w, &QThread::finished, this, [this, w, op, socket]
                {
                    qDebug() << op;
                    double res = w->get();
                    char send[12];
                    *(int *) send = op;
                    *(double *) (send + 4) = res;
                    socket->write(send, 12);
                    socket->waitForBytesWritten();
                }, Qt::QueuedConnection);

            }
        });
    });

}

Server::~Server()
{
    delete ui;
}