//
// Created by mashplant on 18-9-3.
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