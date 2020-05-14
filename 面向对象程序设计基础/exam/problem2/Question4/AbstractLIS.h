#ifndef ABSTRACTLIS_H
#define ABSTRACTLIS_H

#include <vector>

class AbstractLIS
{
  public:
    virtual void solve(const std::vector<int> &input, std::vector<int> &output) = 0;
    virtual ~AbstractLIS() {}
};

#endif // ABSTRACTLIS_H