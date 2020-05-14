#ifndef MST_H
#define MST_H

#include <vector>

struct edge
{
  int startnode;
  int endnode;
  float weight;
};

class GRAPH;

class MST
{
public:
  MST(const GRAPH &igraph);
  ~MST();
  virtual void computeMST() = 0;
  void print(const char *filename) const;

protected:
  const std::vector< std::vector<float> > &my_weight_graph;
  const int n;                 // graph的结点数
  std::vector<edge> tree_edge; // 记录最小生成树的边
};

class MST_Prim : public MST
{
public:
  MST_Prim(const GRAPH &igraph);
  ~MST_Prim();
  void computeMST();  

private:
  std::vector<bool> node_choosen;              // 记录每个结点是否被选过
  std::vector<int> node_pre;                   // 记录每个结点在最短路径中的前驱结点
  std::vector<float> node_distance;            // 记录每个结点到已选结点的最短距离
  void print_Prim(const char *filename) const; // 测试用
};

class MST_Kruskal : public MST
{
public:
  MST_Kruskal(const GRAPH &igraph);
  ~MST_Kruskal();
  void computeMST();

private:
  std::vector<edge> binary_heap;                  // 二叉堆
  bool add_edge(edge minweightedge);              // 判断加边后不构成回路
  std::vector<int> union_find_set;                // 并查集
  void print_Kruskal(const char *filename) const; // 测试用
};

#endif // MST_H