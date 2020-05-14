#include <iostream>
#include "TxtGener.h"
#include "Direct.h"
#include "PublicCompositionMatch.h"
#include <ctime>
using namespace std;

double RuntimeD(Direct &dAlgor)
{
  auto beg = clock();
  dAlgor.matchAll();
  return clock() - beg;
}
double RuntimePubCM(PublicCompositionMatch &PubCM)
{
  auto beg = clock();
  PubCM.matchAll();
  return clock() - beg;
}
int main()
{
  TxtGener generator;
  //This will only generate data when there is no file
  generator.RandomPattern(/*"pattern.txt",1000000000*/);
  generator.RandomArray(/*"array.txt",1000000000*/);

  double time1 = 0;
  Direct dAlgor;
  dAlgor.loadDatas();
  time1 = RuntimeD(dAlgor);

  double time2 = 0;
  PublicCompositionMatch PubCM("pattern.txt", "array.txt");
  time2 = RuntimePubCM(PubCM);

  cout << "Imp = " << (time1 - time2) / time1 << endl;
  //cout << (dAlgor.matches == PubCM.matches) << endl;
  return 0;
}