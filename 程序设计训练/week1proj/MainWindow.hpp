//
// Created by mashplant on 18-8-27.
//

#ifndef MCS_MAINWINDOW_HPP
#define MCS_MAINWINDOW_HPP

#include "MCSFrame.hpp"
#include "ConfigureDialog.hpp"
#include "DesignDialog.hpp"
#include <QMainWindow>
#include <QTimer>
#include <QPointF>

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
    MCSFrame frame;
    ConfigureInfo configure;
    DesignInfo design;
    bool valid = false;
    std::vector<double> result;

    // response to single & double click
    // actually we need atomic int for click_count
    // but in the normal case it doesn't matter
    QTimer click_timer;
    int click_count = 0, click_x, click_y;
    QPoint click_global_pos; // for locating the tool tip

    // current keyboard key, -1 for keyboard not pressing
    int cur_key = -1;

    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void on_single_click();

    void on_double_click();
};

#endif //MCS_MAINWINDOW_HPP