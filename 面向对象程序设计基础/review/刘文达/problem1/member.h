#ifndef MONOCEROS_MEMBER_H
#define MONOCEROS_MEMBER_H
#include <string>
#include <iostream>
class Member {
    std::string name;
    int age;
public:
    Member (std::string nom = "???", int lage = 0);

    void printInfo () const;
};
#endif //MONOCEROS_MEMBER_H
