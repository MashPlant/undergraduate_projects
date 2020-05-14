#include <algorithm>
#include "lis.h"

void LIS::solve(const std::vector<int> &input, std::vector<int> &output)
{
    output.resize(input.size());
    std::vector<int> f(input.size());
    std::fill(f.begin(), f.end(), std::numeric_limits<int>::max());
    for (int i = 0; i < input.size(); ++i)
    {
        auto it = std::lower_bound(f.begin(), f.end(), input[i]);
        *it = input[i];
        output[it - f.begin()] = i;
    }
    output.erase(output.begin() + (std::lower_bound(f.begin(), f.end(), std::numeric_limits<int>::max()) - f.begin()), output.end());
}
