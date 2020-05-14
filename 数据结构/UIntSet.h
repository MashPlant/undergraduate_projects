template <unsigned N, unsigned Bits>
struct UIntSet
{
    typedef unsigned size_type;
    typedef unsigned word_type;
    const static size_type _word_bit = 8 * sizeof(word_type);
    const static size_type _data_len = N * Bits / _word_bit + 1u;
    word_type _data[_data_len] = {};

    struct Ref
    {
        UIntSet *self;
        size_type pos;
        Ref &operator=(const Ref &rhs) { return operator=(rhs.word_type()); }
        Ref &operator=(word_type val) { return self->set(pos, val), *this; }
        operator word_type() const { return self->at(pos); }
    };

    Ref operator[](size_type pos) { return {this, pos}; }
    word_type operator[](size_type pos) const { return at(pos); }
    // note: shift bits out of range [0, size of int) is undefined
    void set(size_type pos, word_type val)
    {
        pos *= Bits, val &= (1u << Bits) - 1u;
        const size_type loc = pos / _word_bit, bit = pos % _word_bit, remain = _word_bit - bit;
        if (remain >= Bits)
            _data[loc] = _data[loc] & ~(((1u << Bits) - 1) << bit) | val << bit;
        else
        {
            _data[loc] = _data[loc] & (1u << bit) - 1u | (val & (1u << remain) - 1u) << bit;
            _data[loc + 1u] = _data[loc + 1u] & ~((1 << Bits - remain) - 1) | (val & ~((1u << remain) - 1u)) >> remain;
        }
    }
    word_type at(size_type pos) const
    {
        pos *= Bits;
        const word_type loc = pos / _word_bit, bit = pos % _word_bit, remain = _word_bit - bit;
        return remain >= Bits ? (_data[loc] & ((1u << Bits) - 1) << bit) >> bit
                              : _data[loc] >> bit | (_data[loc + 1u] & (1 << Bits - remain) - 1) << remain;
    }
    size_type size() const { return N; }
};