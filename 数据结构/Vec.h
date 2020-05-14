#include <cstdlib>
#include <cstring>
#include <cassert>
#include <memory>

template <typename T>
struct Vec
{
    // typedef
    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef int size_type;
    typedef int difference_type;
    typedef pointer iterator;
    typedef const_pointer const_iterator;

    // initial status or after move : capacity = 0, _data = nullptr (be memset-ed to 0)
    pointer _data;
    size_type _size, _capacity;

    // life of object
    Vec(size_type size = 0, const_reference init = value_type()) : _data(nullptr), _size(size), _capacity(0)
    {
        reserve(_size);
        for (pointer it = _data, end = _data + _size; it != end; ++it)
            new (it) value_type(init);
    }
    Vec(const Vec &rhs) : _size(rhs._size), _capacity(rhs._capacity)
    {
        _data = reinterpret_cast<pointer>(malloc(sizeof(value_type) * _capacity));
        for (pointer it = _data, end = _data + _size, rit = rhs._data; it != end; ++it, ++rit)
            new (it) value_type(*rit);
    }
    Vec &operator=(const Vec &rhs)
    {
        if (this != &rhs)
        {
            clear();
            reserve(rhs._size);
            _size = rhs._size;
            for (pointer it = _data, end = _data + _size, rit = rhs._data; it != end; ++it, ++rit)
                new (it) value_type(*rit);
        }
        return *this;
    }
    ~Vec()
    {
        clear();
        free(_data);
    }

    // element access
    reference operator[](size_type pos) { return _data[pos]; }
    const_reference operator[](size_type pos) const { return _data[pos]; }
    reference at(size_type pos)
    {
        assert(pos >= 0 && pos < _size);
        return _data[pos];
    }
    const_reference at(size_type pos) const
    {
        assert(pos >= 0 && pos < _size);
        return _data[pos];
    }
    reference front() { return _data[0]; }
    const_reference front() const { return _data[0]; }
    reference back() { return _data[_size - 1]; }
    const_reference back() const { return _data[_size - 1]; }
    pointer data() { return _data; }
    const_pointer data() const { return _data; }

    // modifiers
    void clear()
    {
        // doesn't free memory
        for (pointer it = _data, end = _data + _size; it != end; ++it)
            it->~value_type();
        _size = 0;
    }
    void insert(iterator pos, const_reference value)
    {
        // `insert and be there`
        int index = pos - _data;
        reserve(_size + 1); // pos may be invalidated here
        pos = _data + index;
        memmove(pos + 1, pos, sizeof(value_type) * (_size - index));
        pos->~value_type();
        new (pos) value_type(value);
        ++_size;
    }
    void erase(iterator pos)
    {
        // equivalent to erase(pos, pos + 1)
        pos->~value_type();
        memmove(pos, pos + 1, sizeof(value_type) * (_data + _size - pos - 1));
        --_size;
    }
    void erase(iterator first, iterator last)
    {
        for (pointer it = first; it != last; ++it)
            it->~value_type();
        memmove(first, last, sizeof(value_type) * (_data + _size - last));
        _size -= last - first;
    }
    void push_back(const_reference value)
    {
        reserve(_size + 1);
        new (_data + _size++) value_type(value);
    }
    void pop_back() { (_data + --_size)->~value_type(); }
    void resize(size_type size, const_reference value = value_type())
    {
        if (size < _size)
        {
            for (pointer it = _data + size, end = _data + _size; it != end; ++it)
                it->~value_type();
        }
        else if (size > _size)
        {
            reserve(size);
            for (pointer it = _data + _size, end = _data + size; it != end; ++it)
                new (it) value_type(value);
        }
        _size = size;
    }

    // capacity
    bool empty() const { return _size == 0; }
    size_type size() const { return _size; }
    void reserve(size_type capacity)
    {
        // standard says that if new capacity is greater than capacity()
        // capacity() can be `greater or equal to` new capacity after reserve
        if (_capacity < capacity)
            _data = reinterpret_cast<pointer>(realloc(_data, sizeof(value_type) * (_capacity = capacity << 1)));
    }
    size_type capacity() const { return _capacity; }
    void shrink_to_fit(size_type capacity)
    {
        if (capacity < _capacity && _size <= capacity)
            _data = reinterpret_cast<pointer>(realloc(_data, sizeof(value_type) * (_capacity = capacity)));
    }

    // iterators
    iterator begin() { return _data; }
    const_iterator begin() const { return _data; }
    iterator end() { return _data + _size; }
    const_iterator end() const { return _data + _size; }
};
