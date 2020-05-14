#include <bits/stdc++.h>
using namespace std;
#include <utility>
namespace comiler
{
template <typename K, typename V, typename Hash, typename Equal, int MaxElem>
class StaticMap
{
    using ValueType = std::pair<K, V>;
    constexpr static int calc(int count)
    {
        int ret = 0;
        while (++ret, count >>= 1)
            ;
        return 1 << ret;
    }
    constexpr static int BucketSize = calc(MaxElem);
    constexpr static unsigned hash(size_t h) { return ((h >> 32) ^ h) & (BucketSize - 1); }
    int size = 0;
    struct Node
    {
        ValueType value;
        int next;
        constexpr Node() : value{}, next(-1) {}
    };
    using Nodes = std::pair<std::array<Node, BucketSize>, std::array<int, BucketSize>>;
    Nodes nodes;

  public:
    template <size_t N>
    constexpr StaticMap(const std::array<ValueType, N> &init) : nodes{} //: nodes(build(init))
    {
        unsigned size = 0, h = 0;
        for (auto pr : init)
        {
            h = hash(Hash()(pr.first));
            nodes.first[size++].value.first = pr.first, nodes.first[size].value.second = pr.second, nodes.first[size].next = nodes.second[h];
            nodes.second[h] = size;
        }
    }
    const ValueType *find(const K &key) const
    {
        for (int cur = nodes.second[hash(Hash()(key))]; ~cur; cur = nodes.first[cur].next)
            if (Equal()(key, nodes.first[cur].value.first))
                return &nodes.first[cur].value;
        return nullptr;
    }
};
}

// namespace comiler
// {
// template <typename K, typename V, typename Hash, typename Equal, int MaxElem>
// class StaticMap
// {
//     using ValueType = std::pair<K, V>;
//     constexpr static int calc(int count)
//     {
//         int ret = 0;
//         while (++ret, count >>= 1)
//             ;
//         return 1 << ret;
//     }
//     constexpr static int BucketSize = calc(MaxElem);
//     constexpr static unsigned hash(size_t h) { return ((h >> 32) ^ h) & (BucketSize - 1); }
//     int size = 0;
//     struct Node
//     {
//         ValueType value;
//         int next;
//         constexpr Node() : value{}, next(-1) {}
//     };
//     using Nodes = std::pair<std::array<Node, BucketSize>, std::array<int, BucketSize>>;
//     Nodes nodes;
//     template <size_t N>
//     constexpr Nodes build(const std::array<ValueType, N> &init)
//     {
//         Nodes ret{};
//         unsigned size = 0, h = 0;
//         for (auto pr : init)
//         {
//             h = hash(Hash()(pr.first));
//             ret.first[size++].value.first = pr.first, ret.first[size].value.second = pr.second, ret.first[size].next = ret.second[h];
//             ret.second[h] = size;
//         }
//         return ret;
//     }

//   public:
//     template <size_t N>
//     constexpr StaticMap(const std::array<ValueType, N> &init) : nodes(build(init)) {}
//     const ValueType *find(const K &key) const
//     {
//         for (int cur = nodes.second[hash(Hash()(key))]; ~cur; cur = nodes.first[cur].next)
//             if (Equal()(key, nodes.first[cur].value.first))
//                 return &nodes.first[cur].value;
//         return nullptr;
//     }
// };
// }
struct MyHash
{
    constexpr size_t operator()(int x) const { return x; }
};

struct MyEqual
{
    constexpr int operator()(int x, int y) const { return x == y; }
};
int main()
{
    constexpr comiler::StaticMap<int, int, MyHash, MyEqual, 10> m{array{make_pair(1, 1), make_pair(2, 2)}};
    getchar();
    return 0;
}