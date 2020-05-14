#include <iostream>
#include <string>
#include "member.h"

Member::Member (std::string nom, int lage): name(nom), age(lage) {};


void Member::printInfo () const {
    std::cout << name << ' ' << age << std::endl;
}