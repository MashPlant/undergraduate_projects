#ifndef BFLIS_H
#define BFLIS_H

#include "AbstractLIS.h"
#include <limits>
#include <algorithm>
#include <cstring>

// It's IMPOSSIBLE to make it with 2^1000
// memoization must be used
// this is a O(n ^ 2) algo
// which is quite brute compared to the O(nlgn) one

class BfLIS : public AbstractLIS
{
  public:
    BfLIS() = default;
    void solve(const std::vector<int> &input, std::vector<int> &output) override
    {
        int ans = 0, arg = 0;
        std::vector<int> dp(input.size());
        std::vector<int> prev(input.size());
        memset(prev.data(), -1, sizeof(int) * prev.size());
        dp[0] = 1;
        for (int i = 1; i < input.size(); ++i)
        {
            int cur = 1;
            for (int j = 0; j < i; ++j)
                if (input[j] < input[i] && dp[j] + 1 > cur)
                    cur = dp[j] + 1, prev[i] = j;
            dp[i] = cur;
            if (ans < cur)
                ans = cur, arg = i;
        }
        for (int i = arg; ~i; i = prev[i])
            output.push_back(i);
        std::reverse(output.begin(), output.end());
    }
};

#endif // BFLIS_H