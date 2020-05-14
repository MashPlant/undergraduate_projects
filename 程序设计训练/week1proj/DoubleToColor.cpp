//
// Created by mashplant on 18-8-28.
//

#include "DoubleToColor.hpp"

const std::vector<QColor> DoubleToColor::i2color = []
{
    std::vector<QColor> ret;
    auto range_check = [](int x)
    { return x > high ? high : x < low ? low : x; };
    int r = low, g = low, b = half;
    // factor (increment or decrement)
    int rF = 0, gF = 0, bF = 1;
    while (true)
    {
        ret.push_back(QColor(r, g, b));
        if (b == high)
            gF = 1; // increment green
        if (g == high)
            bF = -1; // decrement blue
        if (b == low)
            rF = +1; // increment red
        if (r == high)
            gF = -1; // decrement green
        if (g == low && b == low)
            rF = -1; // decrement red
        if (r < half && g == low && b == low)
            break;
        r = range_check(r + rF);
        g = range_check(g + gF);
        b = range_check(b + bF);
    }
    return ret;
}();