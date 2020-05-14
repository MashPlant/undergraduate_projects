#ifndef HASHFINDER_H
#define HASHFINDER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class HashFinder
{
    const static size_t P = 19260817ull;
    std::vector<int> array;
    std::vector<int> pattern;
    size_t h, last;
    void load(const std::string &fileName, std::vector<int> &target)
    {
        std::ifstream fin(fileName);
        target.clear();
        if (!fin)
            std::cerr << "Error opening " << fileName << std::endl, exit(-1);
        int val;
        while (fin >> val)
            target.push_back(val);
        std::cout << "Loaded data from file " << fileName << std::endl;
        std::cout << std::endl;
    }

  public:
    HashFinder(const std::string &patFileName, const std::string &inputFileName)
    {
        load(patFileName, pattern);
        load(inputFileName, array);
        h = pattern[0], last = 1ull;
        for (int i = 1, pLen = static_cast<int>(pattern.size()); i < pLen; ++i)
            h = h * P + pattern[i], last *= P;
    }

    std::vector<int> getAllMatches() const
    {
        const int pLen = static_cast<int>(pattern.size()),
                  aLen = static_cast<int>(array.size());
        if (pLen > aLen)
            return {};
        std::vector<int> ret;
        size_t sh = 0ull;
        for (int i = 0; i < pLen; ++i)
            sh = sh * P + array[i];
        if (sh == h)
            ret.push_back(0);
        for (int i = pLen; i < aLen; ++i)
            if ((sh = (sh - array[i - pLen] * last) * P + array[i]) == h)
                ret.push_back(i - pLen + 1);
        return ret;
    }
};

#endif // HASHFINDER_H