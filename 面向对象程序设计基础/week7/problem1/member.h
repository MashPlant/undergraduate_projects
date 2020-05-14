#ifndef MEMBER_H
#define MEMBER_H
#include <iostream>
#include <string>

class Member
{
    //事实上这种应用场景完全没有必要用到std::string
    //用const char *是完全足够的,当然了std::string_view也是可以的
    const std::string name = "???";
    const int age = 0;

  public:
    Member() = default;
    Member(const char *name, int age);
    void printInfo(std::ostream &os = std::cout) const;
};

#endif