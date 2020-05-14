#include "Graph.h"
#include <cmath>
void Graph::addVertex(double x, double y)
{
    auto len = [](double x, double y) { return sqrt(x * x + y * y); };
    vers.push_back({x, y});
    int cur = vers.size() - 1;
    for (int i = 0; i < cur; ++i)
        edges.push_back({ i, cur, len(vers[i].x - x, vers[i].y - y) });
}