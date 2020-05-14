#include <queue>
#include <cstdio>
#include <cstdlib>
using namespace std;
const int N = 1000 + 5;
struct
{
    int l, r, val;
    int &ch(int x) { return (&l)[x]; }
} buf[N];
int root, alloc;
void ins(int val)
{
    int x = root, p = 0;
    while (x)
    {
        if (buf[x].val == val)
            return;
        p = x;
        x = buf[x].ch(buf[x].val < val);
    }
    (p ? buf[p].ch(buf[p].val < val) : root) = ++alloc;
    buf[alloc].val = val;
}
void inorder_r(int x)
{
    if (!x)
        return;
    inorder_r(buf[x].l);
    printf("%d ", buf[x].val);
    inorder_r(buf[x].r);
}
void inorder_i(int x)
{
    int stk[N], top = 0;
    stk[top++] = x;
    while (top)
    {
        while (true)
        {
            int cur = stk[top - 1];
            if (!cur)
            {
                --top;
                break;
            }
            stk[top++] = buf[cur].l;
        }
        int cur = stk[top - 1];
        printf("%d ", buf[cur].val);
        stk[top++] = buf[cur].r;
    }
}

int main()
{
    for (int i = 0; i < 100; ++i)
        ins(rand() % 100);
    inorder_r(root);
    puts("");
    inorder_i(root);
}