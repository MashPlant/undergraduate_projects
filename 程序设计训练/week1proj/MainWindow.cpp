//
// Created by mashplant on 18-8-27.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "Solver.hpp"
#include "DoubleToColor.hpp"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionConfigure, &QAction::triggered, [this]
    {
        ConfigureDialog cd;
        auto pr = cd.get_configure();
        if (pr.second)
        {
            configure = pr.first, valid = true;
            frame.resize(configure.count);
            result = caluconspeed(configure.count, frame.widths, configure.in1, configure.in2, configure.out1,
                                  configure.out2, configure.out3);
            update();
        }
    });
    connect(ui->actionDesign, &QAction::triggered, [this]
    {
        if (valid)
        {
            DesignDialog dd;
            auto pr = dd.get_design();
            if (pr.second)
            {
                design = pr.first;
                frame.widths = sa(configure.count, configure.in1, configure.in2, configure.out1,
                            configure.out2, configure.out3, design.exp1, design.exp2, design.exp3, design.sa_times);
                result = caluconspeed(configure.count, frame.widths, configure.in1, configure.in2, configure.out1,
                                      configure.out2, configure.out3);
                update();
            }
        }
    });
    connect(&click_timer, &QTimer::timeout, [this]
    {
        click_count = 0;
        click_timer.stop();
        on_single_click();
    });

    ui->verticalSlider->setVisible(false);
    connect(ui->verticalSlider, &QSlider::valueChanged, [this](int value)
    {
        frame.change_width(value / 100.0);
        result = caluconspeed(configure.count, frame.widths, configure.in1, configure.in2, configure.out1,
                              configure.out2, configure.out3);
        update();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void draw_gradient_rect(QPainter &painter, int x, int y, int w, int h)
{
    const int n = 1000;
    const double step = 1.0 / n, step_h = step * h;
    for (int i = 0; i <= n; ++i)
    {
        painter.setPen(DoubleToColor::get(i * step));
        painter.setBrush(DoubleToColor::get(i * step));
        painter.drawRect(x, y + i * step_h, w, step_h);
        if (i % (n / 10) == 0)
            painter.drawText(x + w, y + i * step_h, QString::number(i * step, 'f', 1));
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.translate(width() / 2, height() / 2);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (valid)
        frame.paint(painter, configure, result);
    draw_gradient_rect(painter, 600, -500, 50, 1000);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    click_x = event->x() - width() / 2, click_y = event->y() - height() / 2;
    if (++click_count == 1)
        click_timer.start(400); // wait 400 ms for the next click;
    else // more than one click
    {
        click_count = 0;
        click_timer.stop();
        on_double_click();
    }
}

void MainWindow::on_single_click()
{
    if (!valid)
        return;
    auto pr = frame.select(click_x, click_y);
    if (!pr.second)
        return ui->verticalSlider->setVisible(false);
    ui->verticalSlider->setValue(pr.first * 100);
    ui->verticalSlider->setVisible(true);
}

void MainWindow::on_double_click()
{
    if (!valid)
        return;
    ui->verticalSlider->setVisible(false);
    if (frame.select(click_x, click_y).second)
    {
        frame.change_exist();
        result = caluconspeed(configure.count, frame.widths, configure.in1, configure.in2, configure.out1,
                              configure.out2, configure.out3);
        update();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!valid)
        return;
    if (frame.select(event->x() - width() / 2, event->y() - height() / 2).second)
        QToolTip::showText(event->globalPos(), QString::number(result[frame.selected], 'f', 3));
}
