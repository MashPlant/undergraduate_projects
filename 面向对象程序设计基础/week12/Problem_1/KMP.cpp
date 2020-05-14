#include <fstream>
#include <iostream>
#include <cstdlib>
#include "KMP.h"
using namespace std;

void KMP::loadInput(string fileName, vector<int> &targetLib)
{
    targetLib.clear();
    ifstream fileIn(fileName.c_str());
    if (!fileIn)
    {
        cerr << "Error opening " << fileName << endl;
        exit(-1);
    }
    int val;
    while (fileIn >> val)
    {
        targetLib.push_back(val);
    }
    fileIn.close();

    cout << "Loaded data from file " << fileName << endl;
    for (unsigned i = 0; i < targetLib.size(); ++i)
    {
        cout << " " << targetLib[i];
    }
    cout << endl;
}

KMP::KMP(string patFileName, string inputFileName)
{
    loadInput(patFileName, pattern_);
    loadInput(inputFileName, array_);
    computeNext();
}

// I have rewrite the algorithm
// and make KMP::match const

void KMP::computeNext()
{
    next_.resize(pattern_.size(), 0);
    for (int i = 1, j = 0, sz = pattern_.size(); i < sz; next_[i] = j += pattern_[i] == pattern_[j], ++i)
        while (j && pattern_[i] != pattern_[j])
            j = next_[j - 1];
}

int KMP::match(int start /* = 0*/) const 
{
    for (int i = std::max(0, start), j = 0, l1 = array_.size(), l2 = pattern_.size(); i < l1; ++i)
    {
        while (j && array_[i] != pattern_[j])
            j = next_[j - 1];
        if ((j += array_[i] == pattern_[j]) == l2)
            return i - j + 1;
    }
    return -1;
}

KMP::~KMP()
{
    //dtor
}
