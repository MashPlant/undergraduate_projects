/*
* @Author: Mockingjay
* @Date:   2018-05-21 21:10:23
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-21 23:30:30
*/
#include "naive_algo.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void naive_algo::loadInput(string fileName, vector<int> &targetLib) {
    targetLib.clear();
    ifstream fileIn(fileName.c_str());
    if (!fileIn) {
        cerr << "Error opening " << fileName << endl;
        exit(-1);
    }
    int val;
    while(fileIn >> val) {
        targetLib.push_back(val);
    }
    fileIn.close();

    cout << "Loaded data from file " << fileName << endl;
    for (unsigned i = 0; i < targetLib.size(); ++ i) {
        cout << " " << targetLib[i];
    }
    cout << endl;
}

naive_algo::naive_algo(string patFileName, string inputFileName) {
    loadInput(patFileName, pattern_);
    loadInput(inputFileName, array_);
}

void naive_algo::get_match(IntVec& des_arr) {
    for (int i = 0;i < array_.size() - pattern_.size();++i) {
        bool flag = 1;
        for (int j = 0;j < pattern_.size();++j) {
            if (pattern_[j] == array_[i + j]) {
                continue;
            } else {
                flag = 0;
                break;
            }
        }
        if (flag) {
            des_arr.push_back(i);
        }
    }
}
