#include <cstdio>
#include <random>

struct Node
{
    int key;
    int next[0];
};
const int N = 2000000 + 5;
int buf[N * (1 + 2)], alloc;
int max_level;
void init()
{
    buf[0] = std::numeric_limits<int>::max();
    alloc = 1 + 32; // [0, 1 + 32) is occupied by head
}
Node &get(int x) { return (Node &)buf[x]; }
int max(int a, int b) { return a < b ? b : a; }
std::pair<int, int> new_node(int key)
{
    int level = 1;
    while (rand() & 1)
        ++level;
    int n = alloc;
    alloc += 1 + level;
    buf[n] = key;
    return {n, level};
}
int _find(int key, int *prev)
{
    int x = 0;
    for (int i = max_level - 1; i >= 0; --i)
    {
        while (get(get(x).next[i]).key < key)
            x = get(x).next[i];
        prev[i] = x;
    }
    return get(x).next[0];
}
bool find(int key)
{
    int x = 0;
    for (int i = max_level - 1; i >= 0; --i)
        while (get(get(x).next[i]).key < key)
            x = get(x).next[i];
    x = get(x).next[0];
    return key == get(x).key;
}
void insert(int key)
{
    int prev[32]{};
    int x = _find(key, prev);
    if (key == get(x).key)
        return;
    auto [y, level] = new_node(key);
    max_level = max(max_level, level);
    for (int i = 0; i < level; ++i)
    {
        get(y).next[i] = get(prev[i]).next[i];
        get(prev[i]).next[i] = y;
    }
}
bool erase(int key)
{
    int prev[32]{};
    int x = _find(key, prev);
    if (key == get(x).key)
    {
        for (int i = 0; i < max_level && prev[i]; ++i)
            if (get(prev[i]).next[i] == x)
                get(prev[i]).next[i] = get(x).next[i];
        while (max_level && !(get(0).next[max_level - 1]))
            --max_level;
        return true;
    }
    return false;
}
int main()
{
    for (int i = 0; i < 100; ++i)
        insert(i);
}