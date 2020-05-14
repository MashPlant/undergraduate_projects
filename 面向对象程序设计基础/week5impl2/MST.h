#ifndef MST_H
#define MST_H

#include "Graph.h"
#include "UFS.h"
#include <vector>
#include <algorithm>

template <int N>
class MST
{
    typename Graph<N>::Edges edges;
    std::vector<double> result;
    void dfs(int curV, int curE, double curW, double limit, UFS<N> ufs = UFS<N>());

  public:
    MST(const Graph<N> &g);
    std::vector<double> computeTopKMST(int k);
    double computeMST();
};

#include "MST.cpp"
#endif