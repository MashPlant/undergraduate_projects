#ifndef __MEMBER_H_
#define __MEMBER_H_

#include <string>

class Member {
private:
  /**
   * The name of the guy
   */
  const std::string name;

  /**
   * The age of the guy
   */
  const uint64_t age;

public:
  /**
   * Default constructor with default values
   */
  Member();

  /**
   * Constructor
   */
  Member(const std::string &_name, uint64_t _age);

  /**
   * Print the information of the guy to the console
   */
  void printInfo() const;
};

#endif // __MEMBER_H_
