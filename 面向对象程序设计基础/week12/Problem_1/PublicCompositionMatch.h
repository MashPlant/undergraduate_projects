#ifndef PUBLICCOMPOSITIONMATCH_H
#define PUBLICCOMPOSITIONMATCH_H
#include "KMP.h"
#include <iostream>

class PublicCompositionMatch
{
  public:
    KMP kmp;
    PublicCompositionMatch(std::string patFileName, std::string inputFileName)
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

#endif // PUBLICCOMPOSITIONMATCH_H