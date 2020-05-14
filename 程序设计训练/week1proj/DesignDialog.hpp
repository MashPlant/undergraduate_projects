//
// Created by mashplant on 18-8-30.
//

#ifndef MCS_DESIGNDIALOG_HPP
#define MCS_DESIGNDIALOG_HPP

#include <QDialog>

namespace Ui
{
    class DesignDialog;
}

struct DesignInfo
{
    double exp1, exp2, exp3;
    int sa_times;
};

class DesignDialog : public QDialog
{
Q_OBJECT

public:
    explicit DesignDialog(QWidget *parent = 0);

    std::pair<DesignInfo, bool> get_design();

    ~DesignDialog();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::DesignDialog *ui;
    std::pair<DesignInfo, bool> design{{}, false};

    std::pair<DesignInfo, bool> _get_design();
};

#endif // MCS_DESIGNDIALOG_HPP
