#ifndef GRAPH_H
#define GRAPH_H

#include <array>
#include <utility>
template <int N>
class Graph
{
  public:
    struct Edge
    {
        int u, v;
        double w;
    };
    using Edges = std::array<Edge, N *(N - 1) / 2>;
    using Vertex = std::pair<double, double>;
    using Vers = std::array<Vertex, N>;

  private:
    Edges edges;
    Vers vers;

  public:
    template <typename Generator>
    explicit Graph(Generator gen);
    const Edges &getEdges() const { return edges; }
};

#include "Graph.cpp"
#endif