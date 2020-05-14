#include "ReviewerAssigner.h"
#include <iostream>
#include <random>
#include <ctime>
int main()
{
    srand(time(nullptr));
    ReviewerAssigner ra;
    try
    {
        ra.load();
        ra.output();
    }
    catch(std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}