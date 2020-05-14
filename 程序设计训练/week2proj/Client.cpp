//
// Created by mashplant on 18-9-5.
//

#include "MainWindow.hpp"
#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow client(false);
    client.show();
    return app.exec();
}