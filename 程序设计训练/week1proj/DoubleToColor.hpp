//
// Created by mashplant on 18-8-28.
//

#ifndef MCS_DOUBLETOCOLOR_HPP
#define MCS_DOUBLETOCOLOR_HPP

#include <vector>
#include <QColor>


struct DoubleToColor
{
    const static int low = 0, high = 255, half = (high + 1) / 2;
    const static std::vector<QColor> i2color;

    static QColor get(double x)
    {
        if (!(0 <= x && x <= 1))
            return Qt::black;
        int index = x * i2color.size();
        index -= index == i2color.size();
        return i2color[index];
    }
};

#endif //MCS_DOUBLETOCOLOR_HPP
