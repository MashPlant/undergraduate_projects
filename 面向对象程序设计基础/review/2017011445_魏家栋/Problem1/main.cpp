#include <iostream>
#include <cstdlib>
#include <ctime>
#include "mst.h"
#include "graph.h"

using namespace std;

int main()
{
    const int MAX_POINT_NUM = 20;
    const int MAX_POINT_SIZE = 10000;

    srand(time(NULL));
    
    GRAPH g;
    for (int i = 0; i < MAX_POINT_NUM; i++)
    {
        node newnode;
        newnode.x = rand() % (MAX_POINT_SIZE + 1);
        newnode.y = rand() % (MAX_POINT_SIZE + 1);
        int success = g.add_node(newnode);
        if (success == 1) // 结点重叠
        {
            i--;
        }
    }
    
    g.print("graph.txt");

    MST_Prim m(g);
    m.computeMST();
    m.print("tree.txt");

    MST_Kruskal m2(g);
    m2.computeMST();
    m2.print("tree2.txt");

    return 0;
}