#include <cstdio>
#include <iostream>
#include <iomanip>
#include "mst.h"
#include "graph.h"

using namespace std;

MST_Kruskal::MST_Kruskal(const GRAPH &igraph) : MST(igraph)
{
    // 初始化binary_heap
    binary_heap.reserve(n * (n - 1) / 2 / 2); // 完全图的一半边，提高时间效率
    for (int i = 1; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
            if (my_weight_graph[i][j] != 0) // 连通
            {
                edge newedge = {i, j, my_weight_graph[i][j]};
                binary_heap.push_back(newedge);
            }
        }
    }

    // 初始化binary_find_set
    union_find_set.reserve(n);
    for (int i = 0; i < n; i++)
    {
        union_find_set.push_back(i);
    }
}

MST_Kruskal::~MST_Kruskal()
{
    // do nothing
}

void MST_Kruskal::computeMST()
{
    int m = binary_heap.size();
    // 建堆
    for (int i = (m - 2) / 2; i >= 0; i--)
    {
        int fatherid = i;
        edge fatheredge = binary_heap[fatherid]; // 保留父结点，减少数据交换
        for (int childid = 2 * fatherid + 1; childid < m; fatherid = childid, childid = fatherid * 2 + 1)
        {
            // 选择更小的孩子
            if (childid + 1 < m && binary_heap[childid + 1].weight < binary_heap[childid].weight)
            {
                childid++;
            }

            // 下滤
            if (binary_heap[childid].weight < fatheredge.weight)
            {
                binary_heap[fatherid] = binary_heap[childid];
            }
            else
            {
                break;
            }
        }
        binary_heap[fatherid] = fatheredge;
    }

    // print_Kruskal("kruskal.txt");
    double ans = 0.0;
    while (!binary_heap.empty())
    {
        edge minweightedge = binary_heap[0];
        if (add_edge(minweightedge))
        {
            ans += minweightedge.weight;
            tree_edge.push_back(minweightedge);
        }

        // mst已找到
        if (tree_edge.size() == n - 1)
        {
            break;
        }

        // 删除根节点
        edge tempedge = binary_heap.back(); // 保留尾结点权值，减少数据交换
        binary_heap.pop_back();
        int fatherid = 0;
        int mtemp = binary_heap.size();
        for (int childid = 2 * fatherid + 1; childid < mtemp; fatherid = childid, childid = fatherid * 2 + 1)
        {
            // 选择更小的孩子
            if (childid + 1 < mtemp && binary_heap[childid + 1].weight < binary_heap[childid].weight)
            {
                childid++;
            }

            // 下滤
            if (binary_heap[childid].weight < tempedge.weight)
            {
                binary_heap[fatherid] = binary_heap[childid];
            }
            else
            {
                break;
            }
        }
        binary_heap[fatherid] = tempedge;

        // print_Kruskal("kruskal.txt");
    }
    cout << ans << endl;
    if (binary_heap.empty()) // 图不连通
    {
        cout << "disconnected graph." << endl;
        return;
    }
}

// 判断加边后是否构成回路，可以加边（不构成回路）返回true并加边，否则返回false
bool MST_Kruskal::add_edge(edge minweightedge)
{
    int noderep1 = minweightedge.startnode; // startnode的代表
    while (noderep1 != union_find_set[noderep1])
    {
        noderep1 = union_find_set[noderep1];
    }
    // 路径压缩
    int x1 = minweightedge.startnode;
    while (x1 != noderep1)
    {
        int temp = x1;
        x1 = union_find_set[x1];
        union_find_set[temp] = noderep1;
    }

    int noderep2 = minweightedge.endnode; // endnode的代表
    while (noderep2 != union_find_set[noderep2])
    {
        noderep2 = union_find_set[noderep2];
    }
    // 路径压缩
    int x2 = minweightedge.endnode;
    while (x2 != noderep2)
    {
        int temp = x2;
        x2 = union_find_set[x2];
        union_find_set[temp] = noderep2;
    }

    if (noderep1 == noderep2) // 加边后构成回路
    {
        return false;
    }
    else // 合并两个集合
    {
        union_find_set[noderep1] = noderep2;
        return true;
    }
}

void MST_Kruskal::print_Kruskal(const char *filename) const
{
    freopen(filename, "a", stdout);
    cout << "binary_heap: " << endl;
    for (int i = 0; i < binary_heap.size(); i++)
    {
        cout << "(" << binary_heap[i].startnode << "," << binary_heap[i].endnode << ")"
             << binary_heap[i].weight << " ";
    }
    cout << endl;
    cout << "union_find_set: " << endl;
    for (int i = 0; i < union_find_set.size(); i++)
    {
        cout << union_find_set[i] << " ";
    }
    cout << endl << endl;
    fclose(stdout);
    freopen("CON", "w", stdout);
}