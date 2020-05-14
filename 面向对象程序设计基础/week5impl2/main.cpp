#include "MST.h"
#include <iostream>
#include <random>
#include <ctime>

const int V = 20;

int main()
{
    std::mt19937 mt(time(nullptr));
    std::uniform_real_distribution<> gen(0, 10000.0);
    Graph<V> g([&]() -> Graph<V>::Vertex {return {gen(mt), gen(mt)};});
    MST<V> mst(g);
    for (auto d : mst.computeTopKMST(20))
        std::cout << d << ' ';
    return 0;
}