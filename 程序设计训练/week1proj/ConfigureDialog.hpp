//
// Created by mashplant on 18-8-27.
//

#ifndef MCS_CONFIGUREDIALOG_HPP
#define MCS_CONFIGUREDIALOG_HPP

#include <QDialog>

namespace Ui
{
    class ConfigureDialog;
}

struct ConfigureInfo
{
    int count;
    int in1, in2;
    int out1, out2, out3;
};

class ConfigureDialog : public QDialog
{
Q_OBJECT

public:
    explicit ConfigureDialog(QWidget *parent = 0);

    std::pair<ConfigureInfo, bool> get_configure();

    ~ConfigureDialog();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::ConfigureDialog *ui;
    std::pair<ConfigureInfo, bool> configure{{}, false};

    std::pair<ConfigureInfo, bool> _get_configure();
};

#endif // MCS_CONFIGUREDIALOG_HPP
