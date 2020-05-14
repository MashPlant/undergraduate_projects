#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QKeyEvent>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle(tr("Open Text"));
    if (fileDialog.exec() == QDialog::Accepted)
    {
        QString path = fileDialog.selectedFiles()[0];
        qDebug() << path;
        QFile file(path);
        if (file.open(QFile::ReadOnly))
        {
            QTextStream in(&file);
            in.setCodec("UTF-8");
            ui->textEdit->setText(in.readAll());
        }
    }
}

void MainWindow::on_action_Exit_triggered()
{
    this->close();
}

void MainWindow::on_action_Close_triggered()
{
    ui->textEdit->close();
}

void MainWindow::on_action_Find_triggered()
{
    QString str = QInputDialog::getText(this, "find", "find text");
    qDebug() << str;
    QString text = ui->textEdit->toPlainText();
    int count = text.count(str);
    QMessageBox::information(this, "find information",
                             QString(QString("\"%1\" appears %2 time(s) in text").arg(str).arg(count)),
                             QMessageBox::Ok);
}
