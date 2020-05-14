#include <cstdio>
#include <iostream>
#include "mst.h"
#include "graph.h"

using namespace std;

MST::MST(const GRAPH &igraph)
     : my_weight_graph(igraph.get_weight_graph()), n(igraph.size())
{
    tree_edge.reserve(n - 1);
}

MST::~MST()
{
    // do nothing
}

void MST::print(const char *filename) const
{
    freopen(filename, "w", stdout);
    for (int i = 0; i < tree_edge.size(); i++)
    {
        cout << "(" << tree_edge[i].startnode << "," << tree_edge[i].endnode << ")"
             << tree_edge[i].weight << endl;
    }
    fclose(stdout);
    freopen("CON", "w", stdout);
}