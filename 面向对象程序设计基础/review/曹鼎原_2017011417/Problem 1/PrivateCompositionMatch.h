/*
* @Author: Mockingjay
* @Date:   2018-05-15 20:38:50
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 09:51:13
*/
#ifndef PRIVATE_COMPOSITION_MACTH_H_
#define PRIVATE_COMPOSITION_MACTH_H_

#include "KMP.h"
#include <string>
#include <vector>

typedef std::vector <int> IntVec;

class PrivateCompositionMatch {
private:
    KMP kmp_algo_;
public:
    /**
     * constructor calls the constructor of class KMP
     */
    PrivateCompositionMatch(std::string patFileName, std::string inputFileName) : kmp_algo_(patFileName,
        inputFileName) {}
    /**
     * [get_match description]
     * get the matches.
     * @param des_arr [description]
     * for storing the result.
     */
    void get_match(IntVec& des_arr);

    ~PrivateCompositionMatch() {}

};

#endif  //PRIVATE_COMPOSITION_MACTH_H_
