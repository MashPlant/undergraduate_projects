#ifndef DPLIS_H
#define DPLIS_H

#include "AbstractLIS.h"

class DpLIS : public AbstractLIS
{
  public:
    DpLIS() = default;
    void solve(const std::vector<int> &input, std::vector<int> &output) override
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
};

#endif // DPLIS_H