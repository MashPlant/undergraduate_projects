//
// Created by mashplant on 18-8-27.
//

#include "MCSFrame.hpp"
#include "ConfigureDialog.hpp"
#include "DoubleToColor.hpp"
#include <cmath>
#include <cassert>
#include <QPainter>
#include <QDebug>

const double MCSFrame::default_length = 1600.0;
const double MCSFrame::default_width = 200.0;

const double factor = 14.0; // logic size to screen size

struct Vec2
{
    double x, y;

    Vec2(double x = 0.0, double y = 0.0) : x(x), y(y)
    {}

    Vec2 &operator+=(const Vec2 &rhs)
    { return x += rhs.x, y += rhs.y, *this; }

    friend Vec2 operator+(Vec2 lhs, const Vec2 &rhs)
    { return lhs += rhs; }

    Vec2 &operator-=(const Vec2 &rhs)
    { return x -= rhs.x, y -= rhs.y, *this; }

    friend Vec2 operator-(Vec2 lhs, const Vec2 &rhs)
    { return lhs -= rhs; }

    double len() const
    { return std::sqrt(x * x + y * y); }

    static double dot(const Vec2 &lhs, const Vec2 &rhs)
    { return lhs.x * rhs.x + lhs.y * rhs.y; }

    static double cross(const Vec2 &lhs, const Vec2 &rhs)
    { return lhs.x * rhs.y - lhs.y * rhs.x; }
};

double distance(const Vec2 &p1, const Vec2 &p2, const Vec2 &point) // distance between segment p1-p2 to point
{
    Vec2 d1 = point - p1, d2 = point - p2, l = p2 - p1;
    return Vec2::dot(d1, l) < 0 ? d1.len() :
           Vec2::dot(d2, l) > 0 ? d2.len() :
           std::abs(Vec2::cross(d1, l)) / l.len();
}

void MCSFrame::resize(int size)
{
    n = size;
    widths = std::vector<double>(2 * n * (n - 1) + 5, default_width);
    length = default_length;
}

void MCSFrame::paint(QPainter &painter, ConfigureInfo configure, std::vector<double> result)
{
    int in[] = {configure.in1, configure.in2}, out[] = {configure.out1, configure.out2, configure.out3};
    // paint speed (before scaling)
    for (int i = 0; i < 3; ++i)
    {
        int x = out[i], y = n;
        painter.drawText(((x - floor(n / 2.0) - 0.5) * length + x * default_width) / factor,
                         ((y - floor(n / 2.0) + 0.5) * length + y * default_width) / factor,
                         QString::number(result.end()[i - 3], 'f', 3));
    }
    painter.save();
    painter.scale(1.0 / factor, 1.0 / factor);
    painter.setPen(Qt::NoPen);
    // normalize speed
    double max = *std::max_element(result.begin(), result.end());
    for (auto &x : result)
        x /= max;
    // paint in & out tube
    for (int i = 0; i < 2; ++i) // vertical
    {
        int x = in[i], y = -1;
        painter.setBrush(DoubleToColor::get(result.end()[i - 5]));
        painter.drawRect((x - n / 2) * length + x * default_width - default_width / 2,
                         (y - n / 2) * length + y * default_width,
                         default_width, length);
    }
    for (int i = 0; i < 3; ++i) // vertical
    {
        int x = out[i], y = n - 1;
        painter.setBrush(DoubleToColor::get(result.end()[i - 3]));
        painter.drawRect((x - n / 2) * length + x * default_width - default_width / 2,
                         (y - n / 2) * length + y * default_width,
                         default_width, length);
    }
    // paint inner tube
    for (int i = 0; i < n * (n - 1); ++i) // vertical
    {
        int x = i / (n - 1), y = i % (n - 1);
        painter.setBrush(DoubleToColor::get(result[i]));
        painter.drawRect((x - n / 2) * length + x * default_width - widths[i] / 2,
                         (y - n / 2) * length + y * default_width,
                         widths[i], length);
    }
    for (int i = n * (n - 1); i < 2 * n * (n - 1); ++i) // horizontal
    {
        int x = (i - n * (n - 1)) / n, y = (i - n * (n - 1)) % n;
        painter.setBrush(DoubleToColor::get(result[i]));
        painter.drawRect((x - n / 2) * length + (x + 0.5) * default_width,
                         (y - n / 2) * length + (y - 0.5) * default_width - widths[i] / 2,
                         length, widths[i]);
    }
    // cross dot
    painter.setPen(QPen(Qt::black, default_width / 4));
    painter.setBrush(Qt::NoBrush);
    for (int x = 0; x < n; ++x)
        for (int y = 0; y < n; ++y)
            painter.drawRect((x - n / 2) * length + x * default_width - default_width / 2,
                             (y - n / 2) * length + (y - 1) * default_width,
                             default_width, default_width);
    painter.restore();
}

std::pair<double, bool> MCSFrame::select(int click_x, int click_y)
{
    selected = -1;
    Vec2 click_p(click_x * factor, click_y * factor);
    for (int i = 0; i < n * (n - 1); ++i)
    {
        int x = i / (n - 1), y = i % (n - 1);
        Vec2 start((x - n / 2) * length + x * default_width, (y - n / 2) * length + y * default_width);
        if (distance(start, start + Vec2(0, length), click_p) < default_width * 2)
            selected = i;
    }
    for (int i = n * (n - 1); i < 2 * n * (n - 1); ++i)
    {
        int x = (i - n * (n - 1)) / n, y = (i - n * (n - 1)) % n;
        Vec2 start((x - n / 2) * length + (x + 0.5) * default_width, (y - n / 2) * length + (y - 0.5) * default_width);
        if (distance(start, start + Vec2(length, 0), click_p) < default_width * 2)
            selected = i;
    }
    if (selected == -1)
        return {0.0, false};
    else
    {
        double min, max;
        _get_min_max(min, max);
        return {(widths[selected] - min) / (max - min), true};
    }
}

void MCSFrame::change_exist()
{
    if (selected == -1)
        return;
    widths[selected] = widths[selected] ? 0 : default_width;
}

void MCSFrame::change_width(double width_factor)
{
    if (selected == -1)
        return;
    double min, max;
    _get_min_max(min, max);
    widths[selected] = min + (max - min) * width_factor;
}

void MCSFrame::_get_min_max(double &min, double &max)
{
    min = default_width / 10;
    max = std::numeric_limits<double>::max();
    if (selected < n * (n - 1))
    {
        int x = selected / (n - 1), y = selected % (n - 1);
        for (int nx : {x - 1, x + 1})
            if (0 <= nx && nx < n)
                max = std::min(max, 2 * (length - widths[nx * (n - 1) + y] / 2));
        // no need to sub default_width
        // because of the width of the margin is default_width
    }
    else
    {
        int x = (selected - n * (n - 1)) / n, y = (selected - n * (n - 1)) % n;
        for (int ny : {y - 1, y + 1})
            if (0 <= ny && ny < n)
                max = std::min(max, 2 * (length - widths[x * n + ny + n * (n - 1)] / 2));
    }
}
