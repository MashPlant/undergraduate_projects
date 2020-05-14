//
// Created by mashplant on 18-8-29.
//

#ifndef WEEK1LAB22_MAINWINDOW_HPP
#define WEEK1LAB22_MAINWINDOW_HPP

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
};

#endif //WEEK1LAB22_MAINWINDOW_HPP