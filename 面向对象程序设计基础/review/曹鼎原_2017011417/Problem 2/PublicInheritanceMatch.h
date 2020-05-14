/*
* @Author: Mockingjay
* @Date:   2018-05-15 21:03:08
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 09:50:23
*/
#ifndef PUBLIC_INHERITANCE_MACTH_H_
#define PUBLIC_INHERITANCE_MACTH_H_

#include "KMP.h"
#include <string>
#include <vector>

typedef std::vector <int> IntVec;

class PublicInheritanceMatch : public KMP {
public:
    /**
     * constructor calls the constructor of class KMP
     */
    PublicInheritanceMatch(std::string patFileName, std::string inputFileName) : KMP(patFileName,
        inputFileName) {}
    /**
     * [get_match description]
     * get the matches.
     * @param des_arr [description]
     * for storing the result.
     */
    void get_match(IntVec& des_arr);

    ~PublicInheritanceMatch() {}

};

#endif  //PUBLIC_INHERITANCE_MACTH_H_
