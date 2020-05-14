#ifndef DIRECT_H
#define DIRECT_H
#include <vector>

#define private public
#define protected public

class Direct
{
public:
  Direct();

  void loadDatas(const char *pattern = "pattern.txt", const char *array = "array.txt");

  void matchAll();

  long long getCount() const
  {
    return matches.size();
  }

private:
  bool matched;

  void load(const char *fileName, std::vector<int> &Vector);

  std::vector<int> mPattern, mArray;

  std::vector<int> matches;
};
#endif //DIRECT_H