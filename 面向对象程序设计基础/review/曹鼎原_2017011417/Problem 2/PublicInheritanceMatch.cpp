/*
* @Author: Mockingjay
* @Date:   2018-05-15 21:06:09
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 09:52:45
*/
#include "PublicInheritanceMatch.h"
#include <vector>
/**
 * [PublicInheritanceMatch::get_match description]
 * reused the code from the given KMP
 * because the possibility of the occurrance of pattern is about 1/1000
 * so it's only a "constant problem"(常数问题)
 * therefore I didn't optimize it......
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
