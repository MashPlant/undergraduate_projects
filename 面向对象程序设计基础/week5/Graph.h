#include <vector>
class Graph
{
    struct Vertex
    {
        double x, y;
    };
    struct Edge
    {
        int from, to;
        double w;
    };
    std::vector<Edge> edges;
    std::vector<Vertex> vers;

  public:
    void addVertex(double x, double y);
    int vertexNum() const { return vers.size(); }
    const std::vector<Edge> &getEdges() const { return edges; }
};