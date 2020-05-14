/*
* @Author: Mockingjay
* @Date:   2018-05-15 19:58:06
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 09:51:25
*/
#ifndef PUBLIC_COMPOSITION_MACTH_H_
#define PUBLIC_COMPOSITION_MACTH_H_

#include "KMP.h"
#include <string>
#include <vector>

typedef std::vector <int> IntVec;

class PublicCompositionMatch {
public:
    KMP kmp_algo_;
    /**
     * constructor calls the constructor of class KMP
     */
    PublicCompositionMatch(std::string patFileName, std::string inputFileName) : kmp_algo_(patFileName,
        inputFileName) {}
    /**
     * [get_match description]
     * get the matches.
     * @param des_arr [description]
     * for storing the result.
     */
    void get_match(IntVec& des_arr);

    ~PublicCompositionMatch() {}

};

#endif  //PUBLIC_COMPOSITION_MACTH_H_
