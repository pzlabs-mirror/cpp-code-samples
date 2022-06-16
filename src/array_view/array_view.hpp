#ifndef ARRAY_VIEW_HPP_INCLUDED
#define ARRAY_VIEW_HPP_INCLUDED

#include <cstddef>
#include <cassert>

#include <type_traits>
#include <iterator>
#include <memory>

/// @brief A non-owning view that can refer to a contiguous sequence of objects.
/// Inspired by `std::span<T, std::dynamic_extent>`
template<class T>
class ArrayView {
public:
	using value_type = std::remove_cv_t<T>; // ignore `const` and `volatile` qualifiers here
	using element_type = T;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T&;
	using iterator = T*;
	using const_iterator = const T*;
	using difference_type = ptrdiff_t;
	using size_type = std::size_t;

	ArrayView() noexcept = default;

	template<class It>
	ArrayView(It begin, size_type size) noexcept:
		m_data(std::to_address(begin)),
		m_size(size)
	{ }

	template<class It>
	ArrayView(It begin, It end):
		m_data(std::to_address(begin)),
		m_size(static_cast<size_type>(end - begin))
	{
		assert(end >= begin);
	}

	template<std::size_t N>
	ArrayView(T (&arr)[N]) noexcept:
		m_data(arr),
		m_size(N)
	{ }

	//// Conversion from ArrayView of non-const elements to ArrayView of const elements
	ArrayView(ArrayView<value_type> other) noexcept
			requires (std::is_const_v<T>)
		: // See C++ 20 Concepts
		m_data(other.data()),
		m_size(other.size())
	{ }

	/// All of the special member functions are trivial.
	/// There is no need to explicitly declare them

	/// @note returning non-const iterator from a const function is OK since ArrayView doesn't own
	/// the elements and has pointer semantics, similar to const-pointer vs pointer-to-const
	/// confusion
	iterator begin() const noexcept {
		return m_data;
	}

	const_iterator cbegin() const noexcept {
		return m_data;
	}

	iterator end() const noexcept {
		return m_data + m_size;
	}

	const_iterator cend() const noexcept {
		return m_data + m_size;
	}

	std::reverse_iterator<iterator> rbegin() const noexcept {
		return {begin()};
	}

	std::reverse_iterator<iterator> rend() const noexcept {
		return {end()};
	}

	reference front() const noexcept {
		assert(m_size != 0);
		return *m_data;
	}

	reference back() const noexcept {
		assert(m_size != 0);
		return m_data[m_size - 1];
	}

	reference operator[](size_type index) const noexcept {
		assert(index < m_size);
		return m_data[index];
	}

	pointer data() const noexcept {
		return m_data;
	}

	size_type size() const noexcept {
		return m_size;
	}

	size_type size_bytes() const noexcept {
		return m_size * sizeof(element_type);
	}

	bool empty() const noexcept {
		return m_size == 0;
	}

	ArrayView subview(size_type offset, size_type count) const noexcept {
		assert(offset <= m_size);
		return {m_data + offset, count};
	}

	ArrayView subview(size_type offset) const noexcept {
		assert(offset <= m_size);
		return {m_data + offset, m_size - offset};
	}

private:
	T* m_data = nullptr;
	std::size_t m_size = 0;
};

#endif
