#include "Graph.h"
#include "UFS.h"
class MST 
{
  public:
    using Pair = std::pair<int, int>;
    using Result = std::pair<std::vector<Pair>, double>;
    Result operator()(const Graph &g);
};  