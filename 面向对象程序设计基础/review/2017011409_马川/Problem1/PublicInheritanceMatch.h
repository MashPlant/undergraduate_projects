#ifndef PIM_H
#define PIM_H
#include "KMP.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#define MAXINT 1<<8-1
class PublicInheritanceMatch:public KMP
{
public:
  PublicInheritanceMatch(const char* pattern,const char* array):KMP(pattern,array)
  {
    matched = false;
    matches.clear();
  };

  virtual ~PublicInheritanceMatch(){};
  
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
  };

  long long GetmatchNext(long long start = 0)
  {
    return LLmatch(start);
  }

  long long GetpSize() const
  {
    return pattern_.size();
  };

  long long GetaSize() const
  {
    return array_.size();
  };

  void ReloadInput(const char *pattern, const char *array)
  {
    std::cout << "Reload starts" << std::endl;
    loadInput(pattern, pattern_);
    loadInput(array, array_);
    computeLLNext();
    matched = false;
    matches.clear();
    std::cout << "Reload successed, please match again." << std::endl;
  };

protected:
  //expand the searching scale to long long
  long long LLmatch(long long start = 0);
  void computeLLNext();
  bool matched;//used for matchAll()
  std::vector<int> matches;

private:
  PublicInheritanceMatch(const PublicInheritanceMatch &copy);
  PublicInheritanceMatch &operator=(const PublicInheritanceMatch &other);
};
#endif//PIM_H