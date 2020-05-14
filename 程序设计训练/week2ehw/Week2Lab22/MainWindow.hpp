//
// Created by mashplant on 18-9-8.
//

#ifndef WEEK2LAB22_MAINWINDOW_HPP
#define WEEK2LAB22_MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

struct Server;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void update();

private:
    Ui::MainWindow *ui;
    Server *server;
};

#endif //WEEK2LAB22_MAINWINDOW_HPP