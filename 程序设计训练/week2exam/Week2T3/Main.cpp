//
// Created by mashplant on 18-9-10.
//

#include <QApplication>
#include "Server.hpp"
#include "Client.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Client c;
    Server s;
    c.show();
    s.show();
    return app.exec();
}