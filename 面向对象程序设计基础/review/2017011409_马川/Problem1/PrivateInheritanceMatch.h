#ifndef PRIM_H
#define PRIM_H
#include "KMP.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#ifndef MAXINT
#define MAXINT 1 << 8 - 1
#endif//MAXINT
class PrivateInheritanceMatch : private KMP
{
public:
  PrivateInheritanceMatch(const char *pattern, const char *array) : KMP(pattern, array)
  {
    matched = false;
    matches.clear();
  };

  virtual ~PrivateInheritanceMatch(){};
  //use to process matching, do this to get the operating time
  void matchAll();

  long long GetmatchNext(long long start = 0)
  {
    return LLmatch(start);
  }
  //gets all the matches in the vector
  std::vector<int> GetMatches() const
  {
    if (!matched)
    {
      std::cerr << "The matches have not been done, use the matchAll() first" << std::endl;
      exit(-1);
    }
    else
      return matches;
  };

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
  //expand the computeNext() to long long
  void computeLLNext();
  //expand the match() to long long
  long long LLmatch(long long start = 0);

  bool matched; //used for matchAll()
  std::vector<int> matches;

private:
  PrivateInheritanceMatch(const PrivateInheritanceMatch &copy);
  PrivateInheritanceMatch &operator=(const PrivateInheritanceMatch &other);
};
#endif //PRIM_H