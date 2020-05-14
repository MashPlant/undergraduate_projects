//
// Created by mashplant on 18-9-9.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QString>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include "Worker.hpp"

#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        sockets{},
        active_count(0)
{
    ui->setupUi(this);
}

void MainWindow::server_New_Connect()
{
    qDebug() << "````````````````````````new connect```````````````````````````";
    while (server.hasPendingConnections())
    {
        QTcpSocket *socket = server.nextPendingConnection();
        if (active_count == MAX_CONNECTIONS)
        {
            socket->close();
            ui->logshow->append(QString("too many conn--> %1").arg(active_count));
            return;
        }
        int free_index;
        for (free_index = 0; sockets[free_index]; ++free_index);
        sockets[free_index] = socket;
        ++active_count;
        ui->logshow->append(QString("number of conn--> %1").arg(free_index));
        connect(socket, &QTcpSocket::readyRead, [this, free_index]
        {
            qDebug() << "```````````````````worker start````````````````````";
            auto worker = new Worker<QByteArray>(&MainWindow::socket_Read_Data, this, free_index);
            connect(worker, &Worker<QByteArray>::finished,
                    this, // specify the context in which the lambda will be invoked
                    [this, free_index, worker]
                    {
                        QByteArray data = worker->get();
                        sockets[free_index]->write(data);
                        sockets[free_index]->flush();
                        sockets[free_index]->close();
                        sockets[free_index] = nullptr;
                        --active_count; // no need for mutex
                    }, Qt::QueuedConnection);
            worker->start();
        });

    }
}

QByteArray MainWindow::socket_Read_Data(int sock_idx)
{
    time_t now;
    char timebuf[100];
    int curlen = 0;
    const char *file;
    char buf[40960];

    char content[1024 * 1024];
    QTcpSocket *socket = sockets[sock_idx];

    socket->read(buf, 40960);
    char method[1000], path[1000], protocol[1000];
    memset(path, 0, 1000);
    sscanf(buf, "%[^ ] %[^ ] %[^ ]", method, path, protocol);
    int idx = 0;
    while (protocol[idx] != '\r' && protocol[idx] != '\n' && protocol[idx] != '\0')
        idx++;
    protocol[idx] = '\0';
    qDebug() << "path name =" << path << "\n";


    if (path[0] != '/')
    {
        qDebug() << "Bad filename.";
        return {};
    }
    if (path[1] == '\0')
        file = "index.html";

    file = &(path[1]);

    qDebug() << "Request file-> " << file;

    QFileInfo fi(file);
    if (!fi.exists())
    {
        qDebug() << "no file-> " << file;
        return {};
    }

    memset(content, 0, 1024 * 1024);
    sprintf(&content[curlen], "%s 200 Ok\r\n", protocol);

    curlen = strlen(content);
    sprintf(&content[curlen], "Server: LocalHost\r\n");

    fflush(stdout);
    now = time((time_t *) 0);
    strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
    curlen = strlen(content);
    sprintf(&content[curlen], "Date: %s\r\n", timebuf);
    curlen = strlen(content);
    if (fi.suffix() == "htm" || fi.suffix() == "html")
        sprintf(&content[curlen], "Content-Type: text/html; charset=utf-8\r\n");
    if (fi.suffix() == "jpg")
        sprintf(&content[curlen], "Content-Type: application/x-jpg\r\n");
    curlen = strlen(content);
    sprintf(&content[curlen], "Content-Length: %lld\r\n", fi.size());

    curlen = strlen(content);
    sprintf(&content[curlen], "Last-Modified: %s\r\n", timebuf);
    curlen = strlen(content);
    sprintf(&content[curlen], "Connection: close\r\n");
    curlen = strlen(content);
    sprintf(&content[curlen], "\r\n");
    curlen = strlen(content);

    qDebug() << "Response Head-> " << content << "Head size" << curlen;

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open failed." << endl;
        return {};
    }

    qDebug() << "open success-> " << file;

    f.read(&content[curlen], f.size());
    curlen += f.size();
    f.close();

    qDebug("file =%s, size =%d\n", file, curlen);

    return QByteArray(content, curlen);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    connect(&server, &QTcpServer::newConnection, this, &MainWindow::server_New_Connect);
    server.listen(QHostAddress::Any, 8080);
    ui->logshow->append(QString("HTTP Server started on Port-> %1").arg(server.serverPort()));
    ui->startButton->setEnabled(false);
}
