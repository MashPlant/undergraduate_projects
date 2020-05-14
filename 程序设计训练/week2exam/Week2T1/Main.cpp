//
// Created by mashplant on 18-9-10.
//

#include <QApplication>
#include "Dialog.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Dialog d;
    d.show();
    Server s;
    s.show();
    return app.exec();
}