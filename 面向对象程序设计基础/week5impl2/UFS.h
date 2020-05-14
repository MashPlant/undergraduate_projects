#ifndef UFS_H
#define UFS_H
#include <type_traits>

template <int N>
class UFS
{
    using IntType = typename std::conditional<N < 256, unsigned char, int>::type;
    IntType fa[N];
    int find(int x) { return fa[x] == x ? x : fa[x] = find(fa[x]); }

  public:
    void reset()
    {
        for (int i = 0; i < N; ++i)
            fa[i] = i;
    }
    UFS() { reset(); }
    bool connected(int x, int y) { return find(x) == find(y); }
    void connect(int x, int y)
    {
        int fx = find(x), fy = find(y);
        if (fx != fy)
            fa[fx] = fa[fy];
    }
};

#endif