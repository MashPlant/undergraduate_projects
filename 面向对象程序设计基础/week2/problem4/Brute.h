#include <string>
#include <vector>
#ifndef BRUTE_H
#define BRUTE_H

class Brute
{
    std::string pat;

  public:
    Brute(const std::string &pat) : pat(pat) {}
    std::vector<int> findIn(const std::string &str) const
    {
        std::vector<int> ret;
        for (int i = 0; i < str.size(); ++i)
        {
            int j = 0;
            for (; j < pat.size() && i + j < str.size(); ++j)
                if (pat[j] != str[i + j])
                    break;
            if (j == pat.size())
                ret.push_back(i);
        }
        return ret;
    }
};

inline std::vector<int> stlfind(const std::string &str, const std::string &pat)
{
    std::vector<int> ret;
    int pos = 0;
    while ((pos = str.find(pat, pos)) != std::string::npos)
        ret.push_back(pos++);
    return ret;
}
#endif