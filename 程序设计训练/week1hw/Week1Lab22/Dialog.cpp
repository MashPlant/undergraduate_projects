#include "Dialog.hpp"
#include "ui_Dialog.h"
#include <QDateTime>

int get_day(int year, int month)
{
    const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[month] + (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}


Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QDate current_date = QDateTime::currentDateTime().date();
    result.first.y = current_date.year();
    result.first.m = current_date.month();
    result.first.d = current_date.day();
    set_day_range();
    ui->editYear->setText(QString::number(result.first.y));
    ui->comboMonth->setCurrentIndex(result.first.m - 1);
    ui->comboDay->setCurrentIndex(result.first.d - 1);
    connect(ui->editYear, &QLineEdit::textEdited, [this](const QString &text)
    {
        bool ok = true;
        int y = text.toInt(&ok);
        if (ok)
        {
            result.first.y = y;
            set_day_range();
        }
    });
    connect(ui->comboMonth, (void (QComboBox::*)(int)) &QComboBox::currentIndexChanged, [this](int index)
    {
        result.first.m = index + 1;
        set_day_range();
    });
    connect(ui->comboDay, (void (QComboBox::*)(int)) &QComboBox::currentIndexChanged, [this](int index)
    { result.first.d = index + 1; });
    connect(this, &Dialog::accepted, [this]
    { result.second = true; });
}

void Dialog::set_day_range()
{
    int day = get_day(result.first.y, result.first.m), cur_day = ui->comboDay->count();
    if (day > ui->comboDay->count())
    {
        for (int i = 0; i < day - cur_day; ++i)
            ui->comboDay->addItem(QString::number(cur_day + i + 1));
    }
    else if (day < ui->comboDay->count())
    {
        for (int i = 0; i < cur_day - day; ++i)
            ui->comboDay->removeItem(cur_day - i - 1);
    }
}

Dialog::~Dialog()
{
    delete ui;
}


