#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
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
    while(fileIn >> val)
    {
        targetLib.push_back(val);
    }
    fileIn.close();

    cout << "Loaded data from file " << fileName << endl;
    /*
    for (unsigned i = 0; i < targetLib.size(); ++ i)
    {
        cout << " " << targetLib[i];
    }
    cout << endl;
    */
}

KMP::KMP(string patFileName, string inputFileName)
{
    loadInput(patFileName, pattern_);
    loadInput(inputFileName, array_);
    computeNext();
}

void KMP::computeNext()
{
    next_.clear();
    //there should be a +1 to avoid leak
    next_.assign(pattern_.size() + 1, -1);
    int i = 0;       // next_[i]
    next_[i] = -1;    // next_[0] = -1
    int j = -1;      // position index
    int patLen = (int)(pattern_.size());
    while(i < patLen)
    {
        if(j == -1 || pattern_[i] == pattern_[j])  //j==-1֤���Ѿ���t[0]��ƥ���ˣ���ʱnext[i+1]=0
        {
            i++;
            j++;
            next_[i] = j;
        }
        else
        {
            j = next_[j];
        }
    }
    
}

int KMP::match(int start/* = 0*/)
{
    int i = std::max(0, start);
    int j = 0;
    int patLen = pattern_.size();
    int arrayLen = array_.size();
    if(patLen > arrayLen)
        return -1;
    while(i < arrayLen && j < patLen)
    {
        if(j == -1 || array_[i] == pattern_[j])
        {
            i++;
            j++;
        }
        else
        {
            j = next_[j];
        }
    }
    if (j >= patLen)
        return i-j;
    else
        return -1;
}

KMP::~KMP()
{
    //dtor
}
