#include <cstdio>
#include <iostream>
#include <iomanip>
#include "mst.h"
#include "graph.h"

using namespace std;

const int MAX_DISTANCE = 1000000;

MST_Prim::MST_Prim(const GRAPH &igraph) : MST(igraph)
{
    // 初始化node_choosen，node_pre和node_distance，以0号结点作为初始点
    node_choosen.resize(n, false); // false表示未被选中
    node_choosen[0] = true;
    node_pre.resize(n, -1); // -1表示没有前驱结点
    node_distance.resize(n, MAX_DISTANCE);
    node_distance[0] = 0;
    for (int i = 1; i < n; i++)
    {
        if (my_weight_graph[i][0] != 0) // 连通
        {
            node_pre[i] = 0;
            node_distance[i] = my_weight_graph[i][0];
        }
    }

    // print_Prim("prim.txt");
}

MST_Prim::~MST_Prim()
{
    // do nothing
}

void MST_Prim::computeMST()
{
    if (n == 0) // 空图，特判
    {
        cout << "empty graph." << endl;
        return;
    }
    double ans = 0.0;
    for (int k = 0; k < n - 1; k++) // 最多迭代n-1次
    {
        int minnodeid = -1;
        float mindistance = MAX_DISTANCE;
        // 选择最短路径
        for (int i = 0; i < n; i++)
        {
            if (node_choosen[i] == false && node_distance[i] < mindistance)
            {
                minnodeid = i;
                mindistance = node_distance[i];
            }
        }

        if (minnodeid == -1) // 图不连通
        {
            cout << "disconnected graph." << endl;
            return;
        }

        // 将新选中的边加入tree_edge
        edge newedge = {node_pre[minnodeid], minnodeid, mindistance};
        tree_edge.push_back(newedge);
        ans += mindistance;
        // 更新node_chosen，node_pre和node_distance
        node_choosen[minnodeid] = true;
        for (int i = 0; i < n; i++)
        {
            if (node_choosen[i] == false) // 只需更新未被选中的结点
            {
                float tempdistance = MAX_DISTANCE;
                // 分类讨论，保证落在weight_graph下三角
                if (i > minnodeid && my_weight_graph[i][minnodeid] != 0) // 只需更新连通的结点
                {
                    tempdistance = my_weight_graph[i][minnodeid];
                }
                else if (i < minnodeid && my_weight_graph[minnodeid][i] != 0) // 只需更新连通的结点
                {
                    tempdistance = my_weight_graph[minnodeid][i];
                }
                if (tempdistance < node_distance[i])
                {
                    node_pre[i] = minnodeid;
                    node_distance[i] = tempdistance;
                }
            }
        }

        // print_Prim("prim.txt");
    }

    cout << ans << endl;
}

void MST_Prim::print_Prim(const char *filename) const
{
    freopen(filename, "a", stdout);
    cout << "node_choosen: " << endl;
    for (int i = 0; i < node_choosen.size(); i++)
    {
        cout << "No." << setw(2) << setiosflags(ios::left) << i 
             << setw(2) << resetiosflags(ios::left) << node_choosen[i] << " ";
    }
    cout << endl;
    cout << "node_pre: " << endl;
    for (int i = 0; i < node_pre.size(); i++)
    {
        cout << setw(7) << node_pre[i] << " ";
    }
    cout << endl;
    cout << "node_distance: " << endl;
    for (int i = 0; i < node_distance.size(); i++)
    {
        cout << setw(7) << node_distance[i] << " ";
    }
    cout << endl
         << endl;
}