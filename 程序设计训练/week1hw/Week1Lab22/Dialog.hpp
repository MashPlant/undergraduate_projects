#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui
{
    class Dialog;
}

struct Result
{
    int y, m, d;
};

class Dialog : public QDialog
{
Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);

    ~Dialog();

    std::pair<Result, bool> get_result()
    {
        if (exec() == Accepted)
            return result;
        return {{}, false};
    }

private:
    Ui::Dialog *ui;
    std::pair<Result, bool> result = {};

    void set_day_range();
};

#endif // DIALOG_H
