//
// Created by mashplant on 18-8-29.
//

#ifndef WEEK1LAB3_MAINWINDOW_HPP
#define WEEK1LAB3_MAINWINDOW_HPP

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

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;
    int x, y;
    std::vector<uint8_t> color;
};

#endif //WEEK1LAB3_MAINWINDOW_HPP