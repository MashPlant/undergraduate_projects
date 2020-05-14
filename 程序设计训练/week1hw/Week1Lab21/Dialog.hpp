#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui
{
    class Dialog;
}


struct SearchInfo
{
    QString content;
    bool backward = false;
};

class Dialog : public QDialog
{
Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);

    ~Dialog();

    std::pair<SearchInfo, bool> get_result()
    {
        bool accepted = exec() == Dialog::Accepted;
        return {result, accepted};
    }

private:
    Ui::Dialog *ui;
    SearchInfo result;
};

#endif // DIALOG_H
