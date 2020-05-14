#include <bits/stdc++.h>
using namespace std;
const int N = 100050;
typedef double ld;
struct P
{
    ld x, y;
    bool operator<(const P &a) const { return x < a.x || x == a.x && y < a.y; }
    P operator-(const P &a) const { return (P){x - a.x, y - a.y}; }
    ld operator&(const P &a) const { return x * a.y - y * a.x; }
    ld operator|(const P &a) const { return x * a.x + y * a.y; }
} p[N];

#define cross(a, b, c) ((b - a) & (c - a))

ld dis2(const P &a)
{
    return a.x * a.x + a.y * a.y;
}
#define check(a, b, c, d) (cross(p[a], p[c], p[d]) * cross(p[b], p[c], p[d]) < 0 && cross(p[c], p[a], p[b]) * cross(p[d], p[a], p[b]) < 0)
struct P3
{
    ld x, y, z;
    bool operator<(const P3 &a) const { return x < a.x || x == a.x && y < a.y; }
    P3 operator-(const P3 &a) const { return (P3){x - a.x, y - a.y, z - a.z}; }
    ld operator|(const P3 &a) const { return x * a.x + y * a.y + z * a.z; }
    P3 operator&(const P3 &a) const { return (P3){y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x}; }
} ori[N];
#define gp3(a) \
    (P3) { a.x, a.y, a.x *a.x + a.y *a.y }
bool incir(int a, int b, int c, int d)
{
    P3 aa = gp3(p[a]), bb = gp3(p[b]), cc = gp3(p[c]), dd = gp3(p[d]);
    if (cross(p[a], p[b], p[c]) < 0)
        std::swap(bb, cc);
    return (cross(aa, bb, cc) | (dd - aa)) < 0;
}
int eCnt = 0, last[N], tot, stk[N << 2], ufs[N];

struct E
{
    int to, prev, next;
} e[N << 5];
void add(int x, int y)
{
    e[++eCnt] = (E){y, last[x]}, e[last[x]].next = eCnt, last[x] = eCnt;
    e[++eCnt] = (E){x, last[y]}, e[last[y]].next = eCnt, last[y] = eCnt;
}
void del(int x)
{
    e[e[x].next].prev = e[x].prev;
    e[e[x].prev].next = e[x].next;
    last[e[x ^ 1].to] == x ? last[e[x ^ 1].to] = e[x].prev : 1;
}
void delaunay(int l, int r)
{
    if (r - l <= 2)
    {
        for (int i = l; i < r; i++)
            for (int j = i + 1; j <= r; j++)
                add(i, j);
        return;
    }
    int i, mid = l + r >> 1, ld = 0, rd = 0;
    delaunay(l, mid), delaunay(mid + 1, r);
    for (tot = 0, i = l; i <= r; stk[++tot] = i++)
        while (tot > 1 && cross(p[stk[tot - 1]], p[stk[tot]], p[i]) < 0)
            tot--;
    for (i = 1; i < tot && !ld; i++)
        if (stk[i] <= mid && mid < stk[i + 1])
            ld = stk[i], rd = stk[i + 1];
    for (; add(ld, rd), 1;)
    {
        int id = 0, op = 0;
        for (i = last[ld]; i; i = e[i].prev)
            if (cross(p[ld], p[rd], p[e[i].to]) > 0)
                if (!id || incir(ld, rd, id, e[i].to))
                    op = -1, id = e[i].to;
        for (i = last[rd]; i; i = e[i].prev)
            if (cross(p[rd], p[ld], p[e[i].to]) < 0)
                if (!id || incir(ld, rd, id, e[i].to))
                    op = 1, id = e[i].to;
        if (op == 0)
            break;
        if (op == -1)
        {
            for (i = last[ld]; i; i = e[i].prev)
                if (check(rd, id, ld, e[i].to))
                    del(i), del(i ^ 1), i = e[i].next;
            ld = id;
        }
        else
        {
            for (i = last[rd]; i; i = e[i].prev)
                if (check(ld, id, rd, e[i].to))
                    del(i), del(i ^ 1), i = e[i].next;
            rd = id;
        }
    }
}
struct D
{
    int x, y;
    ld v;
} d[N << 3];
bool operator<(const D &i, const D &j) { return i.v < j.v; }
int gf(int x) { return ufs[x] == x ? x : ufs[x] = gf(ufs[x]); }

mt19937 mt;
uniform_real_distribution<> dis(0, 1000);

int main()
{
    int n, xx, yy;
    n = 5000;
    //scanf("%d", &n);
    for (int i = 1; i <= n; i++)
        xx = dis(mt), yy = dis(mt), p[i] = (P){xx, yy}, ori[i] = (P3){xx, yy, i}, ufs[i] = i;
    clock_t beg = clock();
    std::sort(p + 1, p + 1 + n);
    std::sort(ori + 1, ori + 1 + n);
    delaunay(1, n);
    for (int i = 1; i <= n; i++)
        for (int j = last[i]; j; j = e[j].prev)
            d[++tot] = (D){ori[i].z, ori[e[j].to].z, dis2(p[i] - p[e[j].to])};

    std::sort(d + 1, d + 1 + tot);
    double mst = 0;
    int cnt = 0;
    for (int i = 1; i <= tot && cnt < n - 1; i++)
        if (gf(d[i].x) != gf(d[i].y))
            mst += sqrt(d[i].v), cnt++, ufs[ufs[d[i].x]] = ufs[d[i].y];

    std::cout << clock() - beg << std::endl;
    printf("%.8lf", mst);
}
