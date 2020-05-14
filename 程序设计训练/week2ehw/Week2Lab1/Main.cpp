//
// Created by mashplant on 18-9-5.
//

#include <QApplication>
#include "Dialog.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Dialog d1, d2;
    d1.show(), d2.show();
    return app.exec();
}