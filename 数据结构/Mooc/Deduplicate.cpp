#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
// fuck! 
void *operator new(size_t, void *p) throw() { return p; }
using u64 = unsigned long long;
const u64 P = 19260817;

u64 hash(const char *s)
{
    u64 ret = 0;
    for (; *s; ++s)
        ret = ret * P + *s;
    return ret;
}
namespace detail
{
const float max_load_fac = 0.75f;
const int del_code = 0xdddddddd, empty_code = 0xcdcdcdcd;
// del_code = -572662307, empty_code = -842150451
template <typename T>
struct Hash;
template <>
struct Hash<u64>
{
    u64 operator()(u64 x) const { return x; }
};

template <typename T>
void swap(T &lhs, T &rhs)
{
    T tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}
} // namespace detail

template <typename F, typename S>
struct Pair
{
    F first;
    S second;
};

template <typename K, typename V, typename Hash = detail::Hash<K>>
struct HashMap : private Hash
{
    struct Node
    {
#if NEED_STATUS
        int status = 0;
#endif
        K first;
        V second;

        Node(const K &key, const V &val) : first(key), second(val) {}
        void _del() { this->~Node(), memset(this, detail::del_code, sizeof(int)); }
        bool _is_del() const { return *reinterpret_cast<const int *>(this) == detail::del_code; }
        bool _is_empty() const { return *reinterpret_cast<const int *>(this) == detail::empty_code; }
        bool _has_key() const { return !_is_empty() && !_is_del(); }
    };

    Node *_bucket;
    int _size;
    int _nonempty; // for calculate load_factor
    int _capacity;

    uint64_t _hash_code(const K &key) const
    {
        uint64_t h = this->Hash::operator()(key);
        return (h ^ (h >> 32)) & (_capacity - 1);
    }
    void _expand()
    {
        Node *tmp = reinterpret_cast<Node *>(malloc(sizeof(Node) * (_capacity <<= 1)));
        memset(tmp, detail::empty_code, sizeof(Node) * _capacity);
        for (Node *it = _bucket, *end = _bucket + _capacity / 2; it != end; ++it)
            if (it->_has_key())
            {
                uint64_t h = _hash_code(it->first);
                for (; !tmp[h]._is_empty(); h = (h + 1) & (_capacity - 1))
                    ;
                memcpy(tmp + h, it, sizeof(Node));
            }
        free(_bucket);
        _bucket = tmp;
        _nonempty = _size; // now there is no deleted element
    }

    void swap(HashMap &rhs) { detail::swap(*this, &rhs); }
    HashMap() : _size(0), _nonempty(0), _capacity(8)
    {
        _bucket = reinterpret_cast<Node *>(malloc(sizeof(Node) * _capacity));
        memset(_bucket, detail::empty_code, sizeof(Node) * _capacity);
    }
    HashMap(const HashMap &rhs) : _size(rhs._size), _nonempty(_nonempty), _capacity(_capacity)
    {
        _bucket = reinterpret_cast<Node *>(malloc(sizeof(Node) * _capacity));
        memcpy(_bucket, rhs._bucket, sizeof(Node) * _capacity);
        for (Node *it = _bucket, *end = _bucket + _capacity, *rit = rhs._bucket; it != end; ++it, ++rit)
            if (rit->_has_key())
                new (it) Node(*rit);
    }
    HashMap &operator=(const HashMap &rhs)
    {
        if (this != &rhs)
        {
            HashMap tmp(rhs);
            swap(tmp);
        }
        return *this;
    }
    ~HashMap()
    {
        for (Node *it = _bucket, *end = _bucket + _capacity; it != end; ++it)
            if (it->_has_key())
                it->~Node();
        free(_bucket);
    }

    Pair<Node *, bool> insert(const K &key, const V &val)
    {
        // insert success: return {newly inserted pointer, true}
        // insert fail: return {original pointer, false}
        if (load_fac() > detail::max_load_fac)
            _expand();
        Node *available = nullptr;
        uint64_t h = _hash_code(key);
        for (; !_bucket[h]._is_empty(); h = (h + 1) & (_capacity - 1))
            if (!_bucket[h]._is_del() && _bucket[h].first == key)
                return {_bucket + h, false};
            else if (_bucket[h]._is_del() && !available)
                available = _bucket + h;
        available = available ?: _bucket + h;
        new (available) Node{key, val};
        ++_size, ++_nonempty;
        return {available, true};
    }
    void insert(const Pair<K, V> &pair) { insert(pair.first, pair.second); }

    Node *find(const K &key)
    {
        for (uint64_t h = _hash_code(key); !_bucket[h]._is_empty(); h = (h + 1) & (_capacity - 1))
            if (!_bucket[h]._is_del() && _bucket[h].first == key)
                return _bucket + h;
        return nullptr;
    }
    void erase(const K &key)
    {
        for (uint64_t h = _hash_code(key); !_bucket[h]._is_empty(); h = (h + 1) & (_capacity - 1))
            if (!_bucket[h]._is_del() && _bucket[h].first == key)
                return --_size, _bucket[h]._del();
    }
    V &operator[](const K &key)
    {
        if (load_fac() > detail::max_load_fac)
            _expand();
        Node *available = nullptr;
        uint64_t h = _hash_code(key);
        for (; !_bucket[h]._is_empty(); h = (h + 1) & (_capacity - 1))
            if (!_bucket[h]._is_del() && _bucket[h].first == key)
                return _bucket[h].second;
            else if (_bucket[h]._is_del() && !available)
                available = _bucket + h;
        new (available = available ?: _bucket + h) Node{key, V()};
        ++_size, ++_nonempty;
        return available->second;
    }
    float load_fac() const { return static_cast<float>(_nonempty) / _capacity; }
    Node *end() const { return nullptr; }
};
int main()
{
    int n;
    char name[64];
    HashMap<u64, int> hm;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i)
    {
        scanf("%s", name);
        if (++hm[hash(name)] == 2)
            puts(name);
    }
}