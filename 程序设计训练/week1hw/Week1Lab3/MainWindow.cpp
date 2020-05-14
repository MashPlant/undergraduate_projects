//
// Created by mashplant on 18-8-29.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "Dialog.h"
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QMouseEvent>
#include <cmath>
#include <QMessageBox>

const double max_length = 800.0;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        x(-1), y(-1)
{
    ui->setupUi(this);
    connect(ui->actionConfigure, &QAction::triggered, [this]
    {
        x = -1, y = -1;
        Dialog d(&x, &y);
        d.exec();
        if (x > 0 && y > 0)
        {
            color = std::vector<uint8_t>(x * y);
            update();
        }
    });
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    if (x > 0 && y > 0)
    {
        double factor = max_length / std::max(x, y);
        QPainter painter(this);
        painter.translate(width() / 2, height() / 2);
        painter.scale(factor, -factor);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(QPen(Qt::black, 1 / factor));
        painter.setBrush(Qt::blue);
        for (double j = -y / 2.0; j <= y / 2.0; ++j)
            painter.drawLine(QPointF(-x / 2.0, j), QPointF(x / 2.0, j));
        for (double i = -x / 2.0; i <= x / 2.0; ++i)
            painter.drawLine(QPointF(i, -y / 2.0), QPointF(i, y / 2.0));
        for (int i = 0; i < x; ++i)
            for (int j = 0; j < y; ++j)
                if (color[i * y + j])
                    painter.drawRect(QRectF((i - x / 2.0), (j - y / 2.0), 1, 1));
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (x > 0 && y > 0)
    {
        double factor = max_length / std::max(x, y);
        int clk_x = (event->x() - width() / 2.0) / factor + x / 2.0,
                clk_y = -(event->y() - height() / 2.0) / factor + y / 2.0;
        if (0 <= clk_x && clk_x < x && 0 <= clk_y && clk_y < y)
        {
            color[clk_x * y + clk_y] ^= 1;
            update();
            QMessageBox::information(this,
                                     "You've just clicked",
                                     QString("click position: (%1, %2)").arg(clk_x).arg(clk_y),
                                     QMessageBox::Ok);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}




