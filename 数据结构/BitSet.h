#include <cstring>
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
template <unsigned N>
struct BitSet
{
	using word_type = unsigned long long;
	using size_type = unsigned;
	const static size_type word_size = sizeof(word_type) * 8;
	const static size_type size = (N + word_size - 1) / word_size;

	word_type _bits[size]{};
	void _trim() { _bits[size - 1] &= (1ull << N % word_size) - 1; }

	BitSet() = default;
	BitSet(word_type init) : BitSet() { _bits[0] = init; }
	size_type count() const
	{
		size_type ret = 0;
		for (size_type i = 0; i < size; ++i)
			ret += __builtin_popcountll(_bits[i]);
		return ret;
	}
	bool at(size_type pos) const
	{
		const size_type byte = pos / word_size, bit = pos % word_size;
		return _bits[byte] & 1ull << bit;
	}
	void set(size_type pos, bool val = true)
	{
		const size_type byte = pos / word_size, bit = pos % word_size;
		val ? _bits[byte] |= 1ull << bit : _bits[byte] &= ~1ull << bit;
	}
	void flip(size_type pos)
	{
		const size_type byte = pos / word_size, bit = pos % word_size;
		_bits[byte] ^= 1ull << bit;
	}
	friend BitSet operator<<(BitSet lhs, size_type rhs) { return lhs <<= rhs; }
	friend BitSet operator>>(BitSet lhs, size_type rhs) { return lhs >>= rhs; }
	friend BitSet operator&(BitSet lhs, const BitSet &rhs) { return lhs &= rhs; }
	friend BitSet operator|(BitSet lhs, const BitSet &rhs) { return lhs |= rhs; }
	friend BitSet operator^(BitSet lhs, const BitSet &rhs) { return lhs ^= rhs; }
	friend BitSet operator~(BitSet lhs) { return lhs.flip_all(), lhs; }
	void flip_all()
	{
		for (size_type i = 0; i < size; ++i)
			_bits[i] = ~_bits[i];
		_trim();
	}
	void reset() { memset(_bits, 0, sizeof _bits); }
	BitSet &operator<<=(size_type shift)
	{
		const size_type word_shift = shift / word_size;
		if (word_shift != 0)
			for (size_type pos = size - 1; ~pos; --pos)
				_bits[pos] = word_shift <= pos ? _bits[pos - word_shift] : 0;
		if ((shift %= word_size) != 0)
		{
			for (size_type pos = size - 1; pos > 0; --pos)
				_bits[pos] = (_bits[pos] << shift) | (_bits[pos - 1] >> (word_size - shift));
			_bits[0] <<= shift;
		}
		return *this;
	}
	BitSet &operator>>=(size_type shift)
	{
		const size_type word_shift = shift / word_size;
		if (word_shift != 0)
			for (size_type pos = 0; pos < size; ++pos)
				_bits[pos] = word_shift < size - pos ? _bits[pos + word_shift] : 0;
		if ((shift %= word_size) != 0)
		{
			for (size_type pos = 0; pos < size - 1; ++pos)
				_bits[pos] = (_bits[pos] >> shift) | (_bits[pos + 1] << (word_size - shift));
			_bits[size - 1] >>= shift;
		}
		return *this;
	}
	BitSet &operator&=(const BitSet &rhs)
	{
		for (size_type i = 0; i < size; ++i)
			_bits[i] &= rhs._bits[i];
		return *this;
	}
	BitSet &operator|=(const BitSet &rhs)
	{
		for (size_type i = 0; i < size; ++i)
			_bits[i] |= rhs._bits[i];
		_trim();
		return *this;
	}
	BitSet &operator^=(const BitSet &rhs)
	{
		for (size_type i = 0; i < size; ++i)
			_bits[i] ^= rhs._bits[i];
		_trim();
		return *this;
	}
	struct BitRef
	{
		BitSet *self;
		size_type pos;
		operator bool() const { return self->at(pos); }
		BitRef &operator=(bool b) const { return self->set(pos, b), *this; }
		BitRef &operator=(const BitRef &rhs) const { return self->set(pos, rhs), *this; }
	};
	BitRef operator[](size_type pos) { return {this, pos}; }
	bool operator[](size_type pos) const { return at(pos); }
};