/*
* @Author: Mockingjay
* @Date:   2018-05-15 21:09:51
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 10:36:00
*/
#include "PrivateInheritanceMatch.h"
#include <vector>
/**
 * [PublicInheritanceMatch::get_match description]
 * reused the code from the given KMP
 *
 * @param des_arr [description]
 * the given array to store the result.
 */
void PrivateInheritanceMatch::get_match(IntVec& des_arr) {
    int i = match();
    while(i != -1) {
        des_arr.push_back(i);
        i = match(i + 1);
    }
}
