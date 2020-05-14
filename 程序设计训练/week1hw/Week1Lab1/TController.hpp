//
// Created by mashplant on 18-8-28.
//

#ifndef WEEK1LAB1_TCONTROLLER_HPP
#define WEEK1LAB1_TCONTROLLER_HPP

#include <QObject>

class TController : public QObject
{
Q_OBJECT
    int t_c;
public:
    TController(int t_c = 0, QObject *parent = nullptr) : t_c(t_c), QObject(parent)
    {}

    int get_c() const
    { return t_c; }

    int get_f() const
    { return t_c * 1.8 + 32; }

    void set_c(int t_c)
    {
        if (t_c == this->t_c)
            return;
        this->t_c = t_c;
        emit c_changed(t_c);
        emit f_changed(get_f());
    }

    void set_f(int t_f)
    { set_c((t_f - 32) * (5.0 / 9.0)); }

signals:

    void c_changed(int t_c);

    void f_changed(int t_f);
};


#endif //WEEK1LAB1_TCONTROLLER_HPP
