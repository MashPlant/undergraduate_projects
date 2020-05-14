//
// Created by mashplant on 18-8-27.
//

#ifndef MCS_MCSFRAME_HPP
#define MCS_MCSFRAME_HPP

#include <vector>

struct QPainter;
struct ConfigureInfo;

struct MCSFrame
{
    const static int default_n = 8;
    const static double default_length, default_width;
    // [0, n * (n - 1) vertical
    // [n * (n - 1), 2 * n * (n - 1) horizontal
    std::vector<double> widths; // widths[i] == 0 for i doesn't exist
    double length;
    int n; // 5 <= size <= 8

    int selected = -1;

    MCSFrame() : widths(2 * default_n * (default_n - 1) + 5, default_width), length(default_length), n(default_n)
    {}

    void resize(int size);

    void paint(QPainter &painter, ConfigureInfo configure, std::vector<double> result);

    // for convenience, the coordinate looks like
    //       |
    //       |
    // ------*------>
    //       |
    //       |
    //       \/
    // return : if selection is valid, return {(width[selected] - min) / (max - min), true}
    // else return {0, false}
    std::pair<double, bool> select(int click_x, int click_y);

    // the following 2 func change the status of selected edge
    void change_exist();

    void change_width(double width_factor);

    // put the min and max possible width of the selected edge into `min` and `max`
    void _get_min_max(double &min, double &max);
};

#endif //MCS_MCSFRAME_HPP
