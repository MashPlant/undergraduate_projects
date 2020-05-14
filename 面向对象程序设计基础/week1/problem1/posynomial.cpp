#include "func.h"
#include <stdexcept>
double posynomial(const std::vector<double> &input)
{
    if (input.back() == 0.0)
        throw std::runtime_error("divide by zero");
    double x = 1.0 / input.back();
    double ret = 0;
    for (int i = input.size() - 2; i >= 0; --i)
        ret = ret * x + input[i];
    return ret;
}