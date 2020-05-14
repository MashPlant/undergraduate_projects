#include <iostream> // cout, endl
#include <cstring> // strlen , strcpy
using namespace std;
class bug {
 char* data_;
public:
  bug(const char *str) {
    data_ = new char[strlen(str) + 1];
    strcpy(data_, str);
  }
  bug(const bug& oldBug) {     //add the copy-constructor
    data_ = new char[strlen(oldBug.data()) + 1];
    strcpy(data_, oldBug.data());
  }
  bug& operator= (const bug& oldBug) {    //add operator "="
    if (this == &oldBug) {
      return *this;
    }
    delete data_;
    data_ =  new char[strlen(oldBug.data()) + 1];
    strcpy(data_, oldBug.data());
    return *this;
  }

  ~bug() { delete data_; }

  const char* data() const { return data_; }

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

int main () {
  bug str1 ("2011");
  str1.show();
  test(str1);
  str1.show();
  return 0;
}
