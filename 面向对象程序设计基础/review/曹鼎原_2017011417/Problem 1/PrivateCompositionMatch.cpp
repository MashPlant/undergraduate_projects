/*
* @Author: Mockingjay
* @Date:   2018-05-15 20:46:15
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 10:35:55
*/
#include "PrivateCompositionMatch.h"
#include <vector>
/**
 * [PublicInheritanceMatch::get_match description]
 * reused the code from the given KMP
 *
 * @param des_arr [description]
 * the given array to store the result.
 */
void PrivateCompositionMatch::get_match(IntVec& des_arr) {
    int i = kmp_algo_.match();
    while(i != -1) {
        des_arr.push_back(i);
        i = kmp_algo_.match(i + 1);
    }
}
