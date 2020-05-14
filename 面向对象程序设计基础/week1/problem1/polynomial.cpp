#include "func.h"
double polynomial(const std::vector<double> &input)
{
    double x = input.back();
    double ret = 0;
    for (int i = input.size() - 2; i >= 0;--i)
        ret = ret * x + input[i];
    return ret;
}