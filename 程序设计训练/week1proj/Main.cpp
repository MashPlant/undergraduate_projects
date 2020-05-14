//
// Created by mashplant on 18-8-27.
//

#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setFont(QFont("ubuntu", 12));
    MainWindow w;
    w.show();
    return app.exec();
}