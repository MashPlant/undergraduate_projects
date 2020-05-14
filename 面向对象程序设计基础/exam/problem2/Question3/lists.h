#ifndef LISTS_H
#define LISTS_H

#include <vector>

template <typename T, typename Pred>
class LISTS
{
    Pred pred;

  public:
    LISTS(Pred pred = Pred()) : pred(pred) {}

    void solve(const std::vector<T> &input, std::vector<int> &output)
    {
        // nullptr for inf
        auto cmp = [=](const T *lhs, const T *rhs) {
            if (lhs == nullptr)
                return false;
            if (rhs == nullptr)
                return true;
            return pred(*lhs, *rhs);
        };
        output.resize(input.size());
        std::vector<const T *> f(input.size(), nullptr);
        for (int i = 0; i < input.size(); ++i)
        {
            auto it = std::lower_bound(f.begin(), f.end(), &input[i], cmp);
            *it = &input[i];
            output[it - f.begin()] = i;
        }
        output.erase(output.begin() + (std::lower_bound(f.begin(), f.end(), nullptr, cmp) - f.begin()), output.end());
    }
};

#endif // LISTS_H