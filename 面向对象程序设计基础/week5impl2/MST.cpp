#include "MST.h"

template <int N>
double MST<N>::computeMST()
{
    UFS<N> ufs;
    double sum = 0.0;
    for (int i = 0; i < edges.size(); ++i)
    {
        int u = edges[i].u, v = edges[i].v;
        if (!ufs.connected(u, v))
        {
            ufs.connect(u, v);
            sum += edges[i].w;
        }
    }
    return sum;
}

template <int N>
std::vector<double> MST<N>::computeTopKMST(int k)
{
    double mst = computeMST();
    while (result.size() < k)
    {
        result.clear();
        dfs(1, 0, 0.0, mst += 100.0);
    }
    std::sort(result.begin(), result.end());
    result.resize(k);
    return result;
}

template <int N>
void MST<N>::dfs(int curV, int curE, double curW, double limit, UFS<N> ufs /*= UFS<N>()*/)
{
    if (curV == N)
    {
        if (curW < limit)
            result.push_back(curW);
        return;
    }
    if (curE == edges.size() || curW + edges[curE].w * (N - curV) >= limit)
        return;
    dfs(curV, curE + 1, curW, limit, ufs);
    int u = edges[curE].u, v = edges[curE].v;
    if (!ufs.connected(u, v))
    {
        ufs.connect(u, v);
        dfs(curV + 1, curE + 1, curW + edges[curE].w, limit, ufs);
    }
}

template <int N>
MST<N>::MST(const Graph<N> &g) : edges(g.getEdges())
{
    using Edge = typename Graph<N>::Edge;
    std::sort(edges.begin(), edges.end(),
              [](const Edge &lhs, const Edge &rhs) { return lhs.w < rhs.w; });
}