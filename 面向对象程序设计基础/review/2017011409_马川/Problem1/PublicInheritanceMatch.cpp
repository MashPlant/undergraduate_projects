#include"PublicInheritanceMatch.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
using namespace std;
void PublicInheritanceMatch::computeLLNext()
{
  next_.clear();
  //add +1 to avoid overflow
  next_.assign(pattern_.size()+1, -1);
  long long i = 0;     // next_[i]
  next_[i] = -1; // next_[0] = -1
  long long j = -1;    // position index
  long long patLen = (long long)(pattern_.size());
  while (i < patLen)
  {
    if (j == -1 || pattern_[i] == pattern_[j]) //j==-1֤���Ѿ���t[0]��ƥ���ˣ���ʱnext[i+1]=0
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

long long PublicInheritanceMatch::LLmatch(long long start)
{
  long long i = max((long long)0, start);
  long long j = 0;
  long long patLen = pattern_.size();
  long long arrayLen = array_.size();
  if (patLen > arrayLen)
    return -1;
  while (i < arrayLen && j < patLen)
  {
    if (j == -1 || array_[i] == pattern_[j])
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
    return i - j;
  else
    return -1;
}

void PublicInheritanceMatch::matchAll()
{
  if (!matched)
  {
    matched = true;
    long long j = -1;
    while (LLmatch(j + 1) != -1)
    {
      j = LLmatch(j + 1);
      matches.push_back(j);
    }
  }
  if (matched)
    cout << "Match complete" << endl;
}