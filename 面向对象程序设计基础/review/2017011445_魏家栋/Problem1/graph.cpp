#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "graph.h"

using namespace std;

GRAPH::GRAPH(int n /*= 0*/)
{
    node_graph.reserve(n);
    weight_graph.reserve(n); // 可以预先指定结点数为n
}

GRAPH::~GRAPH()
{
    // do nothing
}

// 增加结点及对应边
// 重叠返回1，正常返回0
int GRAPH::add_node(const node &newnode)
{
    int success = check_node(newnode);
    if (success == 1)
    {
        return 1;
    }
    node_graph.push_back(newnode);
    vector<float> newline;
    newline.reserve(node_graph.size());
    compute_weight(newline);
    weight_graph.push_back(newline);
    return 0;
}

// 计算边权
void GRAPH::compute_weight(vector<float> &newline)
{
    int size = node_graph.size();
    int xx = node_graph.back().x;
    int yy = node_graph.back().y;
    for (int i = 0; i < size; i++)
    {
        float weight = sqrt((node_graph[i].x - xx) * (node_graph[i].x - xx) + (node_graph[i].y - yy) * (node_graph[i].y - yy));
        newline.push_back(weight);
    }
}

// 检查结点是否重叠
// 重叠返回1，正常返回0
int GRAPH::check_node(const node &newnode)
{
    int size = node_graph.size();
    for (int i = 0; i < size; i++)
    {
        if (newnode.x == node_graph[i].x && newnode.y == node_graph[i].y)
        {
            return 1;
        }
    }
    return 0;
}

const vector< vector<float> > &GRAPH::get_weight_graph() const
{
    return weight_graph;
}

const int GRAPH::size() const
{
    return node_graph.size();
}

void GRAPH::print(const char *filename) const
{
    freopen(filename, "w", stdout);
    cout << "node_graph: " << endl;
    for (int i = 0; i < node_graph.size(); i++)
    {
        cout << "No." << i << "(" << node_graph[i].x << "," << node_graph[i].y << ") " << endl;
    }
    cout << endl;

    cout << "weight_graph: " << endl;
    for (int i = 0; i < weight_graph.size(); i++)
    {
        for (int j = 0; j < weight_graph[i].size(); j++)
        {
            cout << setw(7) << weight_graph[i][j] << " ";
        }
        cout << endl;
    }
    fclose(stdout);
    freopen("CON", "w", stdout);
}