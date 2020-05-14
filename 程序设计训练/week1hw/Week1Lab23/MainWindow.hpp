//
// Created by mashplant on 18-8-29.
//

#ifndef WEEK1LAB23_MAINWINDOW_HPP
#define WEEK1LAB23_MAINWINDOW_HPP

#include <QMainWindow>
#include <vector>

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
    std::vector<QPixmap> icons;
    int cur_index = 0;

private slots:
    void load();
};

#endif //WEEK1LAB23_MAINWINDOW_HPP