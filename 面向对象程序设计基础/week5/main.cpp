#include "MST.h"
#include <iostream>
#include <random>
#include <ctime>
int main()
{
    const int V = 20;
    std::mt19937 mt(time(nullptr));
    std::uniform_real_distribution<> gen(0.0, 10000.0);
    Graph g;
    for (int i = 0; i < V;++i)
        g.addVertex(gen(mt), gen(mt));
    MST mst;
    const auto &res = mst(g);
    for (const auto &link : res.first)
        std::cout << link.first << ' ' << link.second << '\n';
    std::cout << res.second << '\n';
}