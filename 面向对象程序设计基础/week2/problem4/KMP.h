#ifndef KMP_H
#define KMP_H
#include <string>
#include <vector>

class KMP
{
    std::string pat;
    std::vector<int> next;

  public:
    KMP(const std::string &pat) : pat(pat), next(pat.size())
    {
        for (int i = 1, j = 0; i < pat.size(); ++i)
        {
            while (j && pat[j] != pat[i])
                j = next[j - 1];
            next[i] = j += pat[j] == pat[i];
        }
    }
    std::vector<int> findIn(const std::string &str) const
    {
        std::vector<int> ret;
        for (int i = 0, j = 0; i < str.size(); ++i)
        {
            while (j && pat[j] != str[i])
                j = next[j - 1];
            if ((j += pat[j] == str[i]) == pat.size())
                ret.push_back(i - j + 1), j = next[j - 1];
        }
        return ret;
    }
};
#endif