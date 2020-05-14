#include "PublicCompositionMatch.h"
#include<fstream>
#include<iostream>
#include<vector>
#include<cstdlib>
using namespace std;
void PublicCompositionMatch::load()
{
  ifstream fin(inPattern),Fin(inArray);
  
  if (!fin)
  {
    cerr << "Error opening " << inPattern << endl;
    exit(-1);
  }
  if (!Fin)
  {
    cerr << "Error opening " << inArray << endl;
    exit(-1);
  }

  int val;
  while (fin >> val)
  {
    pSize++;
  }
  fin.close();
  while(Fin >> val)
  {
    aSize++;
  }
  Fin.close();
}
void PublicCompositionMatch::matchAll()
{
  if(!matched)
  {
    matched = true;
    int j = kmp->match(0);
    while(j != -1)
    {
      matches.push_back(j);
      j = kmp->match(j + 1);
    }
  }
  if(matched)
    cout << "Match complete" << endl;
   
}