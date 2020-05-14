#include "member.h"

Member::Member(const char *name, int age) : name(name), age(age) {}

void Member::printInfo(std::ostream &os /* = std::cout */) const
{
    os << "member : name is " << name
       << " ; age is " << age << std::endl;
}