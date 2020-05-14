#include "MST.h"
#include <algorithm>

MST::Result MST::operator()(const Graph &g)
{
    auto vec = g.getEdges();
    using Edge = decltype(vec)::value_type;
    std::sort(vec.begin(), vec.end(),
              [](const Edge &lhs, const Edge &rhs) { return lhs.w < rhs.w; });
    UFS ufs(g.vertexNum());
    double sum = 0.0;
    std::vector<Pair> tree;
    for (const auto &edge : vec)
    {
        int x = edge.from, y = edge.to;
        if (!ufs.connected(x, y))
        {
            sum += edge.w;
            tree.push_back({x, y});
            ufs.connect(x, y);
        }
    }
    return {tree, sum};
}