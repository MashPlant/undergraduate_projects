#ifndef TXTGENER_H
#define TXTGENER_H
#include<fstream>
#include<ctime>
#include<cstdlib>
class TxtGener
{
public:
  TxtGener(){};
  void RandomPattern(const char *fileName = "pattern.txt", const int length = 10)
  {
    Randombits(fileName, length);
  }
  void RandomArray(const char *fileName = "array.txt", const int length = 10000000)
  {
    Randombits(fileName, length);
  }
protected:
  //generate the data only once, you need to delete the data to generate a new one
  void Randombits(const char *fileName, const int length)
  {
    std::ifstream fin(fileName, std::ifstream::in);
    if(!fin)
    {
      fin.close();
      std::ofstream fout(fileName);
      srand(time(NULL));
      for (int i = 0; i < length; ++i)
      {
        fout << rand() % 2 << " ";
      }
    }
    
  }
};

#endif //TXTGENER_H