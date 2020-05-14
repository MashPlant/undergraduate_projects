#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace detail
{
const float max_load_fac = 0.75f;
const int del_code = 0xdddddddd, empty_code = 0xcdcdcdcd;
// del_code = -572662307, empty_code = -842150451
template <typename T>
struct Hash;

template <>
struct Hash<int>
{
    uint64_t operator()(int x) const { return x; }
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

template <typename K, typename V>
struct HashMap : private detail::Hash<K>
{
    K *_ks;
    V *_vs;
    int _size, _filled, _mask;

    int _hash_code(const K &key) const
    {
        uint64_t h = this->operator()(key);
        return (h ^ (h >> 32)) & _mask;
    }

    static void _del(K &key) { (int &)key = detail::del_code; }

    static bool _is_del(const K &key) { return (const int &)key == detail::del_code; }

    static bool _is_empty(const K &key) { return (const int &)key == detail::empty_code; }

    static bool _has_key(const K &key) { return !_is_empty(key) && !_is_del(key); }

    int size() const { return _size; }

    int capacity() const { return _mask + 1; }

    void _expand()
    {
        _mask = (_mask << 1) + 1;
        int cap = capacity();
        K *k1 = reinterpret_cast<K *>(malloc(sizeof(K) * cap));
        V *v1 = reinterpret_cast<V *>(malloc(sizeof(K) * cap));
        memset(k1, detail::empty_code, sizeof(K) * cap);
        for (int i = 0, old_cap = cap >> 1; i < old_cap; ++i)
            if (_has_key(_ks[i]))
            {
                int h = _hash_code(_ks[i]);
                for (; !_is_empty(k1[h]); h = (h + 1) & _mask)
                    ;
                k1[h] = _ks[i], v1[h] = _vs[i];
            }
        free(_ks), free(_vs);
        _ks = k1, _vs = v1;
        _filled = _size; // now there is no deleted element
    }

    void swap(HashMap &rhs)
    {
        detail::swap(*this, &rhs);
    }

    HashMap() : _size(0), _filled(0), _mask(7)
    {
        int cap = capacity();
        _ks = reinterpret_cast<K *>(malloc(sizeof(K) * cap));
        memset(_ks, detail::empty_code, sizeof(K) * cap);
        _vs = reinterpret_cast<V *>(malloc(sizeof(V) * cap));
    }

    HashMap(const HashMap &rhs) : _size(rhs._size), _filled(rhs._filled), _mask(rhs._mask)
    {
        int cap = capacity();
        _ks = reinterpret_cast<K *>(malloc(sizeof(K) * cap));
        memcpy(_ks, rhs._ks, sizeof(K) * cap);
        _vs = reinterpret_cast<V *>(malloc(sizeof(V) * cap));
        memcpy(_vs, rhs._vs, sizeof(V) * cap);
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
        free(_ks), free(_vs);
    }

    float load_fac() const
    {
        return static_cast<float>(_filled) / capacity();
    }

    Pair<int, bool> insert(const K &key, const V &val)
    {
        // insert success: return {newly inserted index, true}
        // insert fail: return {original index, false}
        if (load_fac() > detail::max_load_fac)
            _expand();
        int avail = -1;
        int h = _hash_code(key);
        for (; !_is_empty(_ks[h]); h = (h + 1) & _mask)
            if (!_is_del(_ks[h]) && _ks[h] == key)
                return {h, false};
            else if (_is_del(_ks[h]) && avail == -1)
                avail = h;
        avail = avail == -1 ? h : avail;
        _ks[avail] = key;
        _vs[avail] = val;
        ++_size, ++_filled;
        return {avail, true};
    }

    void insert(const Pair<K, V> &pair)
    {
        insert(pair.first, pair.second);
    }

    int find(const K &key)
    {
        for (int h = _hash_code(key); !_is_empty(_ks[h]); h = (h + 1) & _mask)
            if (!_is_del(_ks[h]) && _ks[h] == key)
                return h;
        return -1;
    }

    void erase(const K &key)
    {
        for (int h = _hash_code(key); !_is_empty(_ks[h]); h = (h + 1) & _mask)
            if (!_is_del(_ks[h]) && _ks[h] == key)
                return --_size, _del(_ks[h]);
    }

    V &operator[](const K &key)
    {
        if (load_fac() > detail::max_load_fac)
            _expand();
        int avail = -1;
        int h = _hash_code(key);
        for (; !_is_empty(_ks[h]); h = (h + 1) & _mask)
            if (!_is_del(_ks[h]) && _ks[h] == key)
                return _vs[h];
            else if (_is_del(_ks[h]) && avail == -1)
                avail = h;
        avail = avail == -1 ? h : avail;
        _ks[avail] = key;
        _vs[avail] = V();
        ++_size, ++_filled;
        return _vs[avail];
    }

    int end() const { return -1; }

    void reserve(int new_cap)
    {
        int old_cap = capacity();
        _mask = (1 << (32 - __builtin_clz(new_cap))) - 1;
        int cap = capacity();
        K *k1 = reinterpret_cast<K *>(malloc(sizeof(K) * cap));
        memset(k1, detail::empty_code, sizeof(K) * cap);
        V *v1 = reinterpret_cast<V *>(malloc(sizeof(V) * cap));
        for (int i = 0; i < old_cap; ++i)
            if (_has_key(_ks[i]))
            {
                int h = _hash_code(_ks[i]);
                for (; !_is_empty(k1[h]); h = (h + 1) & _mask)
                    ;
                k1[h] = _ks[i];
                v1[h] = _vs[i];
            }
        free(_ks), free(_vs);
        _ks = k1, _vs = v1;
        _filled = _size;
    }
};