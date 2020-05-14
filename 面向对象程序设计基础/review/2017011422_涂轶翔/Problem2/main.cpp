#include <iostream>
#include <cstring>
using namespace std;
class bug {
  char* data_;
public:
  
  //Added definition for operator=(const bug &e) and bug(const bug &e)
  //These solve the problem that default class copying will copy the data_ pointer, causing it to be deleted twice
  bug& operator= (const bug &e) {
    if (&e == this) {
      return *this;
    } else {
      delete []data_;
      data_ = new char[strlen(e.data_) + 1];
      strcpy(data_, e.data_);
      return *this;
    }
  }
  bug(const bug &e) {
    data_ = new char[strlen(e.data_) + 1];
    strcpy(data_, e.data_);
  }
  
  bug(const char *str) {
    data_ = new char[strlen(str) + 1];
    strcpy(data_, str);
  }
  ~bug() {
    delete []data_;
  }
  void show() {
    cout << data_ << endl;
  }
};

void test(bug str1) {
  str1.show();
  bug str2("tsinghua");
  str2.show();
  str2 = str1;
  str2.show();
}

int main() {
  bug str("2011");
  str.show();
  test(str);
  str.show();
  return 0;
}
