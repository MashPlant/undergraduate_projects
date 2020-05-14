#ifndef PRIVATECOMPOSITIONMATCH_H
#define PRIVATECOMPOSITIONMATCH_H

#include "KMP.h"

class PrivateCompositionMatch
{
    KMP kmp;

  public:
    PrivateCompositionMatch(std::string patFileName, std::string inputFileName)
        : kmp(patFileName, inputFileName) {}
    std::vector<int> getAllMatches() const
    {
        std::vector<int> ret;
        int find = 0;
        while (~(find = kmp.match(find)))
            ret.push_back(find++);
        return ret;
    }
};

#endif // PRIVATECOMPOSITIONMATCH_H