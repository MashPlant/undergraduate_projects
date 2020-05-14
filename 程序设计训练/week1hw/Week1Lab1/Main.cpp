//
// Created by mashplant on 18-8-28.
//

#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}