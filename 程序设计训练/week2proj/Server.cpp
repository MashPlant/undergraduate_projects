//
// Created by mashplant on 18-9-3.
//

#include "MainWindow.hpp"
#include <QApplication>
#include <cstdlib>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    // MainWindow w(argc >= 2 && atoi(argv[1]));
    // w.show();
    MainWindow server(true);
    server.show();
    return app.exec();
}
