#ifndef FUNC_H
#define FUNC_H
#include <cmath>
#include <vector>

inline double poly(const std::vector<double> &a,double x)
{
    double ret = 0.0;
    for (int i = a.size() - 1; i >= 0;--i)
        ret = ret * x + a[i];
    return ret;
}
inline double posy(const std::vector<double> &a,double x)
{
    double ret = 0.0;
    for (int i = a.size() - 1; i >= 0;--i)
        ret = ret / x + a[i];
    return ret;
}
inline double polyBrute(const std::vector<double> &a,double x)
{
    double ret = 0.0;
    for (int i = 0; i < a.size();++i)
        ret += a[i] * pow(x, i);
    return ret;
}
inline double posyBrute(const std::vector<double> &a,double x)
{
    double ret = 0.0;
    for (int i = 0; i < a.size(); ++i)
        ret += a[i] * pow(x, -i);
    return ret;
}
#endif