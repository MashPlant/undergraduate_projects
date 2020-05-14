//
// Created by mashplant on 18-8-28.
//

#ifndef WEEK1LAB1_MAINWINDOW_HPP
#define WEEK1LAB1_MAINWINDOW_HPP

#include "TController.hpp"
#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TController controller;
};

#endif //WEEK1LAB1_MAINWINDOW_HPP