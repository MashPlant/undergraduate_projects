#include "Graph.h"
#include <cmath>
template<int N>
template<typename Generator>
Graph<N>::Graph(Generator gen)
{
    for (int i = 0; i < N; ++i)
        vers[i] = gen();
    auto len = [](double x, double y) { return sqrt(x * x + y * y); };
    for (int i = 0, cur = 0; i < N; ++i)
        for (int j = 0; j < i; ++j)
            edges[cur++] = {i, j, len(vers[i].first - vers[j].first, vers[i].second - vers[j].second)};
}