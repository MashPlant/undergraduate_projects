#ifndef KMP_H
#define KMP_H
#include <string>
#include <vector>

// NOTE: This file MUST NOT be modified

class KMP
{
    public:
        KMP(std::string patFileName, std::string inputFileName);
        // search the pattern "pattern_" in array "array_" starting from the index "start", i.e., starting from the element array_[start]
        // If a match is found, then returns the index of the match; otherwise, -1 is returned
        int match(int start = 0);
        virtual ~KMP();
    protected:
        std::vector<int> array_;   // stores the integer (0-1) array
        std::vector<int> pattern_; // stores the integer (0-1) pattern to be searched
        std::vector<int> next_;    // for KMP algorithm
        void computeNext();        // for KMP algorithm
        // load the input from the file
        void loadInput(std::string fileName, std::vector<int> &array); // load in the input file
    private:
        KMP(const KMP& other);
        KMP& operator = (const KMP& other);
};

#endif // KMP_H
