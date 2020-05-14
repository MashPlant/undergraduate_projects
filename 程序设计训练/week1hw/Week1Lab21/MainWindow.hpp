//
// Created by mashplant on 18-8-28.
//

#ifndef WEEK1LAB21_MAINWINDOW_HPP
#define WEEK1LAB21_MAINWINDOW_HPP

#include <QMainWindow>
#include "Dialog.hpp"

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

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    SearchInfo search;
    std::vector<int> search_result;
    int cur_index = -1; // -1 for not searching
};

#endif //WEEK1LAB21_MAINWINDOW_HPP