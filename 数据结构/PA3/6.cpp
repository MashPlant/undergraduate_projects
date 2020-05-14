#include <cstdio>
#include <sys/mman.h>
#include <sys/stat.h>

#pragma GCC optimize("O3")

using u32 = unsigned;
const u32 N = 400000 * 2 + 5;
const u32 P = 19260817;

struct Node
{
  Node *p, *ch[2];
  u32 rev : 1, key : 7, sz : 24;
  u32 hash, rev_hash;
} buf[N], *top = buf, *root = buf;
using Ptr = Node *;
const Ptr nil = buf;

u32 Ps[N];

void init()
{
  Ps[0] = 1;
  for (u32 i = 1; i < N; ++i)
    Ps[i] = Ps[i - 1] * P;
}

template <typename T>
void swap(T &a, T &b)
{
  T t = a;
  a = b;
  b = t;
}

Ptr alloc(Ptr p, u32 key)
{
  Ptr ret = ++top;
  ret->ch[0] = ret->ch[1] = nil;
  ret->key = key, ret->sz = 1, ret->p = p;
  ret->hash = ret->rev_hash = key;
  return ret;
}

void push(Ptr x)
{
  if (x->rev)
  {
    swap(x->ch[0], x->ch[1]);
    swap(x->hash, x->rev_hash);
    x->ch[0]->rev ^= 1;
    x->ch[1]->rev ^= 1;
    x->rev = 0;
  }
}

void update(Ptr x)
{
  Ptr l = x->ch[0], r = x->ch[1];
  push(l), push(r);
  x->sz = 1 + l->sz + r->sz;
  x->hash = l->hash * Ps[r->sz + 1] + x->key * Ps[r->sz] + r->hash;
  x->rev_hash = r->rev_hash * Ps[l->sz + 1] + x->key * Ps[l->sz] + l->rev_hash;
}

void rot(Ptr x)
{
  Ptr p = x->p, g = p->p;
  u32 l = x == p->ch[0];
  x->p = g;
  (g != nil ? g->ch[p == g->ch[1]] : root) = x;
  (p->ch[l ^ 1] = x->ch[l])->p = p;
  (x->ch[l] = p)->p = x;
  update(p);
}

void splay(Ptr x, Ptr goal = nil)
{
  for (Ptr p = x->p, g = p->p; x->p != goal; p = x->p, g = p->p)
  {
    if (g != goal)
    { // optimize rotate
      Ptr gp = g->p;
      u32 l = x == p->ch[0], same = (x == p->ch[0]) == (p == g->ch[0]);
      x->p = gp;
      (gp != nil ? gp->ch[g == gp->ch[1]] : root) = x;
      (p->ch[l ^ 1] = x->ch[l])->p = p;
      if (same)
      {
        (x->ch[l] = g)->p = x;
      }
      else
      {
        (x->ch[l] = p)->p = x;
        (g->ch[l] = x->ch[l ^ 1])->p = g;
        (x->ch[l ^ 1] = g)->p = x;
      }
      update(p), update(g); // p is not higher that g
    }
    else
    {
      rot(x);
      break;
    }
  }
  update(x);
}

Ptr kth(u32 k) // return pointer of kth
{
  Ptr x = root;
  while (true)
  {
    push(x);
    if (k <= x->ch[0]->sz)
      x = x->ch[0];
    else if (k <= x->ch[0]->sz + 1)
      return x;
    else
      k -= x->ch[0]->sz + 1, x = x->ch[1];
  }
}

struct IO
{
  char *p_in;

  IO()
  {
#ifndef _OJ_
    freopen("in.txt", "r", stdin);
#endif
    struct stat st;
    int fn = fileno(stdin);
    fstat(fn, &st);
    p_in = (char *)mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fn, 0);
  }

  operator u32()
  {
    u32 v = 0;
    while (*p_in < '0')
      ++p_in;
    do
      v = v * 10 + *p_in++ - '0';
    while (*p_in > ' ');
    return v;
  }
} io;

void build(Ptr &rt, int l, int r)
{
  if (l == r)
  {
    (rt = ++top)->key = io.p_in[l - 1], rt->sz = 1;
    rt->ch[0] = rt->ch[1] = nil;
    rt->hash = rt->rev_hash = rt->key;
    return;
  }
  if (l > r)
    return;
  u32 mid = l + r >> 1;
  (rt = ++top)->key = io.p_in[mid - 1];
  build(rt->ch[0], l, mid - 1);
  build(rt->ch[1], mid + 1, r);
  rt->ch[0]->p = rt->ch[1]->p = rt;
  update(rt);
}

int main()
{
  init();
  u32 n = io, m = io, ans = 0;
  io.p_in[0] = io.p_in[n + 1] = 0;
  build(root, 1, n + 2);
  io.p_in += n + 1;
  while (m--)
  {
    u32 op = io, p = io;
    switch (op)
    {
    case 1:
    {
      u32 c = *++io.p_in;
      ++io.p_in;
      splay(kth(p + 1));
      Ptr r = kth(p + 2);
      splay(r, root);
      r->ch[0] = alloc(r, c);
      update(r), update(root);
      break;
    }
    case 2:
    {
      splay(kth(p));
      Ptr r = kth(p + 2);
      splay(r, root);
      r->ch[0] = 0;
      update(r), update(root);
      break;
    }
    case 3:
    {
      u32 q = io;
      splay(kth(p));
      Ptr r = kth(q + 2);
      splay(r, root);
      r->ch[0]->rev = 1;
      break;
    }
    case 4:
    {
      u32 q = io, len = io;
      u32 h1, h2, h3, h4;
      {
        splay(kth(p));
        Ptr r = kth(p + len + 1);
        splay(r, root);
        h1 = r->ch[0]->hash;
        h2 = r->ch[0]->rev_hash;
      }
      {
        splay(kth(q));
        Ptr r = kth(q + len + 1);
        splay(r, root);
        h3 = r->ch[0]->hash;
        h4 = r->ch[0]->rev_hash;
      }
      ans += h1 == h3 && h2 == h4;
      break;
    }
    }
  }
  printf("%d\n", ans);
}