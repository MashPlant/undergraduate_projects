#include<fstream>
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include"Direct.h"
using namespace std;

Direct::Direct()
{
  mPattern.clear();
  mArray.clear();
  matched = false;
}

void Direct::load(const char *fileName, vector<int> &Vector)
{
  ifstream fin(fileName);

  if(!fin)
  {
    cerr << "Error opening " << fileName << endl;
    exit(-1);
  }
  cout << "Start loading data from file " << fileName << endl;
  int tmp;
  while(fin >> tmp)
  {
    Vector.push_back(tmp);
  }

  fin.close();

  cout << "Loaded data from file " << fileName << endl;
}

void Direct::loadDatas(const char* pattern,const char* array)
{
  load(pattern, mPattern);
  load(array, mArray);
}

void Direct::matchAll()
{
  if(!matched)
  {
    matched = true;
    int i = 0, j = 0;
    bool flag = true;

    int Asize = mArray.size();
    int Psize = mPattern.size();

    while (i < Asize - Psize)
    {
      flag = true;
      for (j = 0; j < Psize && flag; ++j)
      {
        if (mPattern[j] != mArray[i + j])
          flag = false;
      }
      if (flag)
        matches.push_back(i);
      ++i;
    }
  }
  if(matched)
    cout << "Match finished" << endl;
}

