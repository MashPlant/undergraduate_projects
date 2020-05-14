#include <iostream>
#include <random>
#include "../src/YaoMST.h"
#include "../src/PrimMST.h"
#include "../src/KruskalMST.h"

using namespace std;

mt19937 mt;
uniform_real_distribution<> dis(0, 100);

int main()
{
    vector<Point2d> vec;
    for (int i = 0; i < 20000; ++i)
        vec.push_back({dis(mt), dis(mt)});
    {
        auto beg = clock();
        YaoMST y(vec);
        cout << clock() - beg << endl;
    }
}