/*
* @Author: Mockingjay
* @Date:   2018-05-21 20:45:26
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 09:56:39
*/
#ifndef NAIVE_ALGO_
#define NAIVE_ALGO_

#include <string>
#include <vector>

typedef std::vector <int> IntVec;

class naive_algo {
private:
    std::vector<int> array_;   // stores the integer (0-1) array
    std::vector<int> pattern_; // stores the integer (0-1) pattern to be searched
public:
    /**
     * constructor.
     * load the pattern and the array from file.
     */
    naive_algo(std::string, std::string);
    /**
     * [loadInput description]
     * load the input in the given file to the array
     * @param fileName [description]
     * the given file.
     * @param array    [description]
     * the given array.
     */
    void loadInput(std::string fileName, std::vector<int> &array);
    /**
     * [get_match description]
     * the naive algorithm for sequence matching.
     * simply compare each character.
     * @param des_arr [description]
     * for storing the result.
     */
    void get_match(IntVec& des_arr);

    ~naive_algo() {}
};

#endif  //NAIVE_ALGO_
