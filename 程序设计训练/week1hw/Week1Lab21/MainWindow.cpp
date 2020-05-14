//
// Created by mashplant on 18-8-28.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QAction>
#include <QDebug>
#include <QKeyEvent>

std::vector<int> find_all(const QString &str, const QString &pat)
{
    std::vector<int> ret;
    int beg = 0, res;
    while (~(res = str.indexOf(pat, beg)))
        ret.push_back((beg = res)++);
    return ret;
}

void move_n(QTextEdit *edit, int n, bool backward)
{
    QTextCursor cur = edit->textCursor();
    cur.clearSelection();
    cur.movePosition(backward ? QTextCursor::Left : QTextCursor::Right, QTextCursor::MoveAnchor, n);
    edit->setTextCursor(cur);
}

void select_n(QTextEdit *edit, int n)
{
    QTextCursor cur = edit->textCursor();
    cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, n);
    cur.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, n);
    edit->setTextCursor(cur);
}

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionSearch->setShortcut(QKeySequence::Find);
    ui->textEdit->installEventFilter(this);
    connect(ui->actionSearch, &QAction::triggered, [this]
    {
        Dialog dialog;
        auto pr = dialog.get_result();
        if (pr.second)
        {
            search = pr.first;
            QString content = ui->textEdit->toPlainText();
            search_result = find_all(content, search.content);
            if (!search_result.empty())
            {
                if (search.backward)
                {
                    cur_index = search_result.size() - 1;
                    ui->textEdit->moveCursor(QTextCursor::End);
                    move_n(ui->textEdit, content.size() - search_result[cur_index], true);
                    select_n(ui->textEdit, search.content.size());
                }
                else
                {
                    cur_index = 0;
                    ui->textEdit->moveCursor(QTextCursor::Start);
                    move_n(ui->textEdit, search_result[cur_index], false);
                    select_n(ui->textEdit, search.content.size());
                }
            }
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->textEdit && event->type() == QEvent::KeyPress)
    {
        auto press = static_cast<QKeyEvent *>(event);
        auto clear = [this]
        {
            cur_index = -1;
            QTextCursor cur = ui->textEdit->textCursor();
            cur.clearSelection();
            ui->textEdit->setTextCursor(cur);
        };
        if (cur_index != -1 && press->key() == Qt::Key_Tab)
        {
            if (search.backward)
            {
                if (cur_index == 0)
                    clear();
                else
                {
                    move_n(ui->textEdit, search_result[cur_index] - search_result[cur_index - 1], true);
                    select_n(ui->textEdit, search.content.size());
                    --cur_index;
                }
            }
            else
            {
                if (cur_index == search_result.size() - 1)
                    clear();
                else
                {
                    move_n(ui->textEdit, search_result[cur_index + 1] - search_result[cur_index], false);
                    select_n(ui->textEdit, search.content.size());
                    ++cur_index;
                }
            }
            return true;
        }
    }
    return false;
}
