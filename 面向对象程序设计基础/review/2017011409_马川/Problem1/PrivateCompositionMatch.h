#ifndef PRCM_H
#define PRCM_H
#include"KMP.h"
#include<vector>
#include<iostream>
#include<cstdlib>
#define MAXINT 1<<8-1
class PrivateCompositionMatch
{
public:
  PrivateCompositionMatch(const char *pattern, const char *array)
  {
    kmp = new KMP(pattern, array);
    pSize = 0, aSize = 0;
    inPattern = pattern;
    inArray = array;
    load();
    if(pSize > MAXINT)
    {
      std::cerr << "The pattern is too long, please keep it in Interger";
      exit(-1);
    }
    if(aSize > MAXINT)
    {
      std::cerr << "The array is too long, please keep it in Interger";
      exit(-1);
    }
    matched = false;
    matches.clear();
  };
  virtual ~PrivateCompositionMatch()
  {
    delete kmp;
  }

  //match all through the array, do this to get the operating time
  void matchAll();
  //gets all the matches in the vector
  std::vector<int> GetMatches() const
  {
    if(!matched)
    {
      std::cerr << "The matches have not been done, use the matchAll() first" << std::endl;
      exit(-1);
    }
    else
      return matches;
  }
  
  int matchNext(int start = 0)
  {
    return kmp->match(start);
  }

  long long GetpSize() const
  {
    return pSize;
  }

  long long GetaSize() const
  {
    return aSize;
  }

  void ReloadInput(const char *pattern = NULL, const char *array = NULL)
  {
    std::cout << "Reload starts" << std::endl;
    if (pattern == NULL)
    {
      pattern = inPattern;
    }
    if (array == NULL)
    {
      array = inArray;
    }
    inPattern = pattern;
    inArray = array;
    pSize = 0, aSize = 0;
    load();
    delete kmp;
    kmp = new KMP(pattern, array);
    std::cout << "Reload successed, please match again." << std::endl;
  }

protected:
  
  //get the size of pattern and array which can be accessed
  void load();
  long long pSize, aSize;


  bool matched;//used for matchAll()
  std::vector<int> matches;

  const char *inPattern, *inArray;
  
private:
  KMP *kmp;
  PrivateCompositionMatch(const PrivateCompositionMatch &copy);
  PrivateCompositionMatch &operator=(const PrivateCompositionMatch &other);
  
};
#endif//PUCM_H