#include <iostream>
#include "member.h"
using namespace std;

Member::Member()
  : name("???"), age(0) {}

Member::Member(const string &_name, uint64_t _age)
  : name(_name), age(_age) {}

void Member::printInfo() const {
  cout<<name<<": "<<age<<endl;
}
