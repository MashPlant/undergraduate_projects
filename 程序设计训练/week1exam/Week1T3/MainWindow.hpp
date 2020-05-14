//
// Created by mashplant on 18-9-3.
//

#ifndef WEEK1T3_MAINWINDOW_HPP
#define WEEK1T3_MAINWINDOW_HPP

#include <QMainWindow>
#include <QSignalMapper>
#include <QMediaPlayer>

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

    void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::MainWindow *ui;
    QMediaPlayer player;

private slots:
    void keyPressed(int);
};

#endif //WEEK1T3_MAINWINDOW_HPP
