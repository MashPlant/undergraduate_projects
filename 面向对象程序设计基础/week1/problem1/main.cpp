#include "func.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "arguments are too few" << std::endl;
        return 1;
    }
    std::vector<double> input(argc - 1);
    for (int i = 1; i < argc; ++i)
    {
        std::istringstream in(argv[i]);
        in >> input[i - 1];
    }
    std::cout << "polynomial answer is : " << polynomial(input) << std::endl;
    try
    {
        std::cout << "posynomial answer is : " << posynomial(input) << std::endl;
    }
    catch (std::runtime_error err)
    {
        std::cout << err.what() << std::endl;
        return 1;
    }
    return 0;
}