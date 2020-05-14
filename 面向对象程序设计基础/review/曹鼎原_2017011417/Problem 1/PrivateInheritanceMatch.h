/*
* @Author: Mockingjay
* @Date:   2018-05-15 21:09:44
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 09:51:17
*/
#ifndef PRIVATE_INHERITANCE_MACTH_H_
#define PRIVATE_INHERITANCE_MACTH_H_

#include "KMP.h"
#include <string>
#include <vector>

typedef std::vector <int> IntVec;

class PrivateInheritanceMatch : private KMP {
public:
    /**
     * constructor calls the constructor of class KMP
     */
    PrivateInheritanceMatch(std::string patFileName, std::string inputFileName) : KMP(patFileName,
        inputFileName) {}
    /**
     * [get_match description]
     * get the matches.
     * @param des_arr [description]
     * for storing the result.
     */
    void get_match(IntVec& des_arr);

    ~PrivateInheritanceMatch() {}

};

#endif  //PRIVATE_INHERITANCE_MACTH_H_
