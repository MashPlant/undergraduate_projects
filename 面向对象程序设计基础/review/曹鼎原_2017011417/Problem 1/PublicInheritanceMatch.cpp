/*
* @Author: Mockingjay
* @Date:   2018-05-15 21:06:09
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 10:36:06
*/
#include "PublicInheritanceMatch.h"
#include <vector>
/**
 * [PublicInheritanceMatch::get_match description]
 * reused the code from the given KMP
 *
 * @param des_arr [description]
 * the given array to store the result.
 */
void PublicInheritanceMatch::get_match(IntVec& des_arr) {
    int i = match();
    while(i != -1) {
        des_arr.push_back(i);
        i = match(i + 1);
    }
}
