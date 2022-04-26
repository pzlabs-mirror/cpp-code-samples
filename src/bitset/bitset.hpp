#ifndef BITSET_HPP_INCLUDED
#define BITSET_HPP_INCLUDED

#include <cstddef>
#include <cstdint>

#include <ostream>
#include <istream>

/// @brief A set of unique nsigned numbers that uses a dynamically sized sequence of bits
/// to represent the elements of the set
/// @warning BitSet is ill-suited for sparse sets with a large max element
class BitSet {
public:
	/// @brief A wrapper that acts like a reference to a specific bit in the BitSet
	///
	/// This workardound is needed because in C++ bits are not natively addressable
	class BitReference {
	public:
		BitReference(BitSet& set, size_t pos): m_set(&set), m_pos(pos) { }

		explicit operator bool() const {
			return m_set->get(m_pos);
		}

		BitReference& operator=(bool value) {
			m_set->set(m_pos, value);
			return *this;
		}

	private:
		BitSet* m_set;
		size_t m_pos;
	};

	/// @brief Default constructor
	BitSet(): m_data(nullptr), m_size(0) { }

	/// @brief Copy constructor
	BitSet(const BitSet& other);

	/// @brief Copy assignment operarator
	BitSet& operator=(const BitSet& other);

	/// @brief Move constructor [extra]
	BitSet(BitSet&& other) noexcept:
		m_data(other.m_data),
		m_size(other.m_size)
	{
		other.m_data = nullptr;
		other.m_size = 0;
	}

	/// @brief Move assignment operator [extra]
	BitSet& operator=(BitSet&& other) noexcept {
		if (this == &other) {
			return *this;
		}

		delete[] m_data;

		m_data = other.m_data;
		m_size = other.m_size;
		other.m_data = nullptr;
		other.m_size = 0;

		return *this;
	}

	/// @brief Destructor
	~BitSet();

	/// @brief Check whether a given element is contained in the set.
	/// Alternatively, get the value of the bit at the position `pos`
	///
	/// @note Defined in the header to make this function inlinable
	bool get(size_t pos) const {
		const size_t byteIdx = pos / 8;
		if (byteIdx >= m_size) {
			return false;
		}
		const size_t bitIdx = pos % 8;
		return m_data[byteIdx] & (1u << bitIdx);
	}

	/// @brief Add or remove a given element to/from the set.
	/// Alternatively, set the value of the bit at the position `pos`
	void set(size_t pos, bool value = true);

	/// TODO: Add flip() function

	/// @brief Equivalent to `get(pos)`
	bool operator[](size_t pos) const {
		return get(pos);
	}

	/// @brief operator[] that also supports bit assignment as in `bitset[pos] = value`
	/// @note This version can only be called on non-const objects
	BitReference operator[](size_t pos) {
		return {*this, pos};
	}

	/// @brief Remove all elements from the set. Alternatively, set all the bits to zero
	void clear();

	/// @brief Equality operator
	friend bool operator==(const BitSet& a, const BitSet& b);
	/// @brief Nonequality operator
	friend bool operator!=(const BitSet& a, const BitSet& b) {
		return !(a == b);
	}

	/// @brief Construct a union of two sets
	friend BitSet operator|(const BitSet& a, const BitSet& b);
	/// @brief Construct an intersection of two sets
	friend BitSet operator&(const BitSet& a, const BitSet& b);
	/// @brief Construct a symmetric difference of two sets
	friend BitSet operator^(const BitSet& a, const BitSet& b);

	/// @brief Swap the contents of two sets
	friend void swap(BitSet& a, BitSet& b) noexcept {
		using std::swap;

		swap(a.m_data, b.m_data);
		swap(a.m_size, b.m_size);
	}

	/// @brief Write to the stream `output` in the format of `{value1, value2, ..., valueN}`
	friend std::ostream& operator<<(std::ostream& output, const BitSet& bitset);
	/// @brief Read from the stream `input` in the format of `value1 value2 ... valueN`
	friend std::istream& operator>>(std::istream& input, BitSet& bitset);

private:
	uint8_t* m_data;
	size_t m_size; // TODO: replace byte count with bit count
};

#endif
