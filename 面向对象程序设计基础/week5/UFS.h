#include <memory>
class UFS
{
    std::unique_ptr<int[]> fa, rank;
    int find(int x) { return fa[x] == x ? x : fa[x] = find(fa[x]); }

  public:
    UFS(int N) : fa(new int[N]), rank(new int[N])
    {
        for (int i = 0; i < N;++i)
            fa[i] = i;
    }
    bool connected(int x, int y) { return find(x) == find(y); }
    void connect(int x,int y)
    {
        int fx = find(x), fy = find(y);
        if (fx != fy)
        {
            if (rank[fx] < rank[fy])
                fa[fx] = fa[fy];
            else 
            {
                 fa[fy] = fa[fx];
                 rank[fx] += rank[fx] == rank[fy];
            }
        }
    }
};