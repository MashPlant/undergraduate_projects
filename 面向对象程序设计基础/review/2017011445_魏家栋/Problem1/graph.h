#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

// 结点struct
struct node
{
  int x;
  int y;
};

class GRAPH
{
public:
  GRAPH(int n = 0);
  ~GRAPH();
  int add_node(const node &newnode);
  const std::vector< std::vector<float> > &get_weight_graph() const;
  const int size() const; // 结点数
  void print(const char *filename) const;

private:
  std::vector<node> node_graph;                 // 记录结点位置
  std::vector< std::vector<float> > weight_graph; // 下三角矩阵，记录边权
  void compute_weight(std::vector<float> &newline);
  int check_node(const node &newnode);
};

#endif // GRAPH_H