#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::Form *ui;
    int value;
    enum
    {
        Stopped,
        Started,
        Paused,
    };
    int status = Stopped;
    QTimer timer;
};

#endif // FORM_H
