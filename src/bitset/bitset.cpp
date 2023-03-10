#include "bitset/bitset.hpp"

#include <cstring>

#include <iomanip>

BitSet::BitSet(const BitSet& other):
	m_size(other.m_size)
{
	if (other.m_size != 0) {
		m_data = new uint8_t[other.m_size];
		std::memcpy(m_data, other.m_data, other.m_size); // Note the std:: namespace
	}
}

BitSet::~BitSet() {
	delete[] m_data;
}

void BitSet::set(size_t pos, bool value) {
	const size_t byteIdx = pos / 8;
	if (byteIdx >= m_size) {
		// We ran out of storage and need to allocate a new buffer
		const size_t newSize = byteIdx + 1;
		// The new buffer is created uninitialized. We will owerwrite it later anyway
		uint8_t* newData = new uint8_t[newSize];
		if (m_data) {
			std::memcpy(newData, m_data, m_size);
		}
		std::memset(newData + m_size, 0, newSize - m_size);
		delete[] m_data;
		m_data = newData;
		m_size = newSize;
	}

	// Branchless bit assignment. Compare with the conditional version:
	// https://godbolt.org/z/3fTfor58b [extra]
	const size_t bitIdx = pos % 8;
	m_data[byteIdx] &= static_cast<uint8_t>(~(1 << bitIdx)); // clear the old bit value
	m_data[byteIdx] |= static_cast<uint8_t>(value * (1 << bitIdx)); // set the new bit value
}

void BitSet::clear() {
	if (m_data) {
		std::memset(m_data, 0, m_size);
	}
}

BitSet& BitSet::operator=(const BitSet& other) {
	if (this == &other) {
		return *this;
	}

	if (m_size != other.m_size) {
		delete[] m_data;
		// Reset data members to make sure that BitSet stays in a valid state even if memory
		// allocation (`new uint8_t[..]`) fails throwing `std::bad_alloc` [extra]
		m_data = nullptr;
		m_size = 0;

		if (other.m_size != 0) {
			m_data = new uint8_t[other.m_size];
			m_size = other.m_size;
		}
	}

	if (m_data) {
		std::memcpy(m_data, other.m_data, m_size);
	}

	return *this;
}

bool operator==(const BitSet& a, const BitSet& b) {
	size_t commonSize;
	size_t longerSize;
	const uint8_t* longerData;
	if (a.m_size >= b.m_size) {
		commonSize = b.m_size;
		longerSize = a.m_size;
		longerData = a.m_data;
	}
	else {
		commonSize = a.m_size;
		longerSize = b.m_size;
		longerData = b.m_data;
	}

	for (size_t i = 0; i < commonSize; ++i) {
		if (a.m_data[i] != b.m_data[i])
			return false;
	}

	for (size_t i = commonSize; i < longerSize; ++i) {
		if (longerData[i] != 0)
			return false;
	}

	return true;
}

BitSet operator|(const BitSet& a, const BitSet& b) {
	BitSet result;
	size_t commonSize;
	size_t longerSize;
	const uint8_t* longerData;
	if (a.m_size >= b.m_size) {
		commonSize = b.m_size;
		longerSize = a.m_size;
		longerData = a.m_data;
	}
	else {
		commonSize = a.m_size;
		longerSize = b.m_size;
		longerData = b.m_data;
	}

	result.m_data = longerSize != 0 ? new uint8_t[longerSize] : nullptr;
	result.m_size = longerSize;
	if (result.m_data) {
		for (size_t i = 0; i < commonSize; ++i) {
			result.m_data[i] = a.m_data[i] | b.m_data[i];
		}
		// Handle the rest of the bytes
		std::memcpy(&result.m_data[commonSize], &longerData[commonSize],
			longerSize - commonSize);
	}

	return result;
}

BitSet operator&(const BitSet& a, const BitSet& b) {
	BitSet result;
	const size_t commonSize = a.m_size >= b.m_size ? b.m_size : a.m_size;

	result.m_data = commonSize != 0 ? new uint8_t[commonSize] : nullptr;
	result.m_size = commonSize;
	if (result.m_data) {
		for (size_t i = 0; i < commonSize; ++i) {
			result.m_data[i] = a.m_data[i] & b.m_data[i];
		}
	}

	return result;
}

BitSet operator^(const BitSet& a, const BitSet& b) {
	BitSet result;
	size_t commonSize;
	size_t longerSize;
	const uint8_t* longerData;
	if (a.m_size >= b.m_size) {
		commonSize = b.m_size;
		longerSize = a.m_size;
		longerData = a.m_data;
	}
	else {
		commonSize = a.m_size;
		longerSize = b.m_size;
		longerData = b.m_data;
	}

	// TODO: Missed optimization: don't allocate more than necessary. First find the supposed max
	// element of the newly created set
	result.m_data = longerSize != 0 ? new uint8_t[longerSize] : nullptr;
	result.m_size = longerSize;
	if (result.m_data) {
		for (size_t i = 0; i < commonSize; ++i) {
			result.m_data[i] = a.m_data[i] ^ b.m_data[i];
		}
		// Handle the rest of the bytes
		std::memcpy(&result.m_data[commonSize], &longerData[commonSize],
			longerSize - commonSize);
	}

	return result;
}

std::ostream& operator<<(std::ostream& output, const BitSet& bitset) {
	const size_t bitCount = bitset.m_size * 8;
	output << "{";
	bool isFirstElement = true;
	for (size_t i = 0; i < bitCount; ++i) {
		if (bitset.get(i)) {
			if (!isFirstElement) {
				output << ", ";
			}
			isFirstElement = false;
			output << i;
		}
	}
	output << "}";
	return output;
}

std::istream& operator>>(std::istream& input, BitSet& bitset) {
	bitset.clear();
	size_t value{};
	while (input >> value) {
		if (input.peek() == '\n') {
			input.setstate(std::ios::eofbit);
		}
		bitset.set(value);
	}
	return input;
}
