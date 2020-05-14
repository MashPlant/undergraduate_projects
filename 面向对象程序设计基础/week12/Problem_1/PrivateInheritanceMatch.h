#ifndef PRIVATEINHERITANCEMATCH_H
#define PRIVATEINHERITANCEMATCH_H

#include "KMP.h"

class PrivateInheritanceMatch : private KMP
{
  public:
    using KMP::KMP;
    std::vector<int> getAllMatches() const 
    {
        std::vector<int> ret;
        for (int i = 0, j = 0, l1 = array_.size(), l2 = pattern_.size(); i < l1; ++i)
        {
            while (j && array_[i] != pattern_[j])
                j = next_[j - 1];
            if ((j += array_[i] == pattern_[j]) == l2)
                ret.push_back(i - j + 1), j = next_[j - 1];
        }
        return ret;
    }
};

#endif // PRIVATEINHERITANCEMATCH_H