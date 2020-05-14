#include "Form.h"
#include "ui_Form.h"
#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QPolygon>

Form::Form(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::Form)
{
    ui->setupUi(this);
    value = 0;
    ui->lcdNumber->setDigitCount(10); 
    ui->lcdNumber->setMode(QLCDNumber::Dec);  
    ui->buttonPause->setEnabled(false);
    ui->buttonZero->setEnabled(false);
    connect(&timer, &QTimer::timeout, [this]
    {
        ++value;
        QTime time;
        time.setHMS(0, value / 60, value % 60);
        ui->lcdNumber->display(time.toString(time.toString("hh:mm:ss")));
        update();
    });
    connect(ui->buttonStart, &QPushButton::clicked, [this]
    {
        if (status == Stopped || status == Paused)
        {
            status = Started;
            timer.start(1000);
            ui->buttonStart->setEnabled(false);
            ui->buttonPause->setEnabled(true);
            ui->buttonZero->setEnabled(false);
        }
    });
    connect(ui->buttonPause, &QPushButton::clicked, [this]
    {
        if (status == Started)
        {
            status = Paused;
            ui->buttonStart->setEnabled(true);
            ui->buttonPause->setEnabled(false);
            ui->buttonZero->setEnabled(true);
            timer.stop();
        }
    });
    connect(ui->buttonZero, &QPushButton::clicked, [this]
    {
        if (status == Paused)
        {
            status = Stopped;
            ui->buttonStart->setEnabled(true);
            ui->buttonPause->setEnabled(false);
            ui->buttonZero->setEnabled(false);
            value = 0;
            timer.stop();
            QTime time;
            time.setHMS(0, 0, value);
            ui->lcdNumber->display(time.toString("mm:ss"));
            update();
        }
    });
}

Form::~Form()
{
    delete ui;
}

void Form::paintEvent(QPaintEvent *event)
{
    int extent = width() - 40;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(width() / 2, height() / 2);
    painter.drawEllipse(-extent / 2, -extent / 2, extent, extent);
    QPen pen = painter.pen();
    pen.setWidth(5);
    painter.setPen(pen);
    for (int i = 0; i < 60; ++i)
    {
        if (i % 5 == 0)
        {
            pen.setWidth(10);
            painter.setPen(pen);
        }
        painter.drawLine(0, -extent / 2 + 5, 0, -extent / 2 + 20);
        if (i % 5 == 0)
        {
            pen.setWidth(5);
            painter.setPen(pen);
        }
        painter.rotate(6);
    }

    painter.rotate(value * 6);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);

    {
        QPolygon polygon;
        polygon << QPoint(extent * 0.02, 0)
                << QPoint(-extent * 0.02, 0)
                << QPoint(0, -extent * 0.46);
        painter.drawPolygon(polygon);
    }
    {
        QPolygon polygon;
        polygon << QPoint(extent * 0.02, 0)
                << QPoint(-extent * 0.02, 0)
                << QPoint(0, extent * 0.06);
        painter.drawPolygon(polygon);
    }
}
