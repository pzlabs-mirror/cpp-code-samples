#ifndef LIST_HPP_INCLUDED
#define LIST_HPP_INCLUDED

#include <cstddef>
#include <cassert>

#include <iterator>

/// A doubly linked list should support iteration in both directions, i.e. its iterator should
/// conform to.BidirectionalIterator requirements. The implementation of such iterator would be
/// trivial except for one special case, decrementing the end iterator: `--end()`.
/// Naive implementation with `nullptr` as a marker of the end of a list doesn't work,
/// since iterator that stores only a null pointer can't go back to the previous node.
/// We have the following options to solve this problem:
///   a) Still use `nullptr` to indicate the last node. By doing so, we will either
///      - lose the ability to decrement `end()`, effectively turning doubly linked list into a
///        singly linked list
///      - need to store the pointer to the parent list container in the iterator, inceasing
///        iterator size and adding an unwanted branch in `iterator::operator--()`
///   b) Add an empty sentinel node so that `m_tail->next` can point to it. The sentinel node would
///      have its `prev` pointer correctly pointing back to `*m_tail`.
///      The disadvantages of additional node are obvious: wasted memory, slightly reduced
///      performance because of one more memory allocation call, and a default-constructed list
///      can no longer be allocation-free
///      This approach is used by Microsoft STL (https://github.com/microsoft/STL/blob/main/stl/inc/list#L798)
///   c) Point from the last node to the list container itself. Instead of storing two
///      separate pointers `m_head` and `m_tail`, list would store an internal node header
///      of the same type as used by the list node. Doing this way, `List::m_ptrs->prev` points to
///      the last node, replacing the `List::m_tail` member, while `List::m_ptrs->next` points to
///      the first node, replacing the `List::m_head` member. Note that in the container we only
///      store a node header that contains just two pointers and not a value, while the proper
///      list node class will need to inherit from the header and also store a `T` value as
///      a member.
///      Compared to non-STL linked list design, the internal node method has zero overhead in
///      terms of memory and negligible overhead in terms of performance. There are, however,
///      a few downsides. Move constructor and move assignment operator can't just copy the contents
///      of the other object and have to reassign the pointers to its own internal node header.
///      Also, `const_cast` or `mutable` is neccessary to construct non-const iterators. Overall,
///      the implementation becomes more cumbersome and complicated.
///      This approach is used by EASTL and libstdc++
/// For the purposes of this example we will implement option c) based on internal node header.

template<typename T>
struct ListNodeHeader {
	ListNodeHeader* prev{};
	ListNodeHeader* next{};
};

/// @brief A single node of a doubly linked list
template<typename T>
struct ListNode: ListNodeHeader<T> {
	T value;
};

template<typename T>
class ListConstIterator;

template<typename T>
class ListIterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using pointer = T*;
	using reference = T&;
	using difference_type = std::ptrdiff_t;

	ListIterator() noexcept = default;
	explicit ListIterator(ListNodeHeader<T>* node): m_node{node} { }

	reference operator*() const noexcept { return static_cast<ListNode<T>*>(m_node)->value; }
	pointer operator->() const noexcept { return &static_cast<ListNode<T>*>(m_node)->value; }

	ListIterator& operator++() noexcept {
		m_node = m_node->next;
		return *this;
	}

	ListIterator operator++(int) noexcept {
		ListIterator tmp = *this;
		m_node = m_node->next;
		return tmp;
	}

	ListIterator& operator--() noexcept {
		m_node = m_node->prev;
		return *this;
	}

	ListIterator operator--(int) noexcept {
		ListIterator tmp = *this;
		m_node = m_node->prev;
		return tmp;
	}

	friend bool operator==(ListIterator a, ListIterator b) noexcept {
		return a.m_node == b.m_node;
	}

	friend bool operator!=(ListIterator a, ListIterator b) noexcept {
		return a.m_node != b.m_node;
	}

private:
	ListNodeHeader<T>* m_node{};

	template<typename U> friend class List;
	friend class ListConstIterator<T>;
};

template<typename T>
class ListConstIterator {
public:
	using iterator = ListIterator<T>;
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using pointer = const T*;
	using reference = const T&;
	using difference_type = std::ptrdiff_t;

	ListConstIterator() noexcept = default;
	explicit ListConstIterator(ListNodeHeader<T>* node): m_node{node} { }
	ListConstIterator(ListIterator<T> other): m_node{other.m_node} { }

	reference operator*() const noexcept { return static_cast<const ListNode<T>*>(m_node)->value; }
	pointer operator->() const noexcept { return &static_cast<const ListNode<T>*>(m_node)->value; }

	ListConstIterator& operator++() noexcept {
		m_node = m_node->next;
		return *this;
	}

	ListConstIterator operator++(int) noexcept {
		ListIterator tmp = *this;
		m_node = m_node->next;
		return tmp;
	}

	ListConstIterator& operator--() noexcept {
		m_node = m_node->prev;
		return *this;
	}

	ListConstIterator operator--(int) noexcept {
		ListIterator tmp = *this;
		m_node = m_node->prev;
		return tmp;
	}

	friend bool operator==(ListConstIterator a, ListConstIterator b) noexcept {
		return a.m_node == b.m_node;
	}

	friend bool operator!=(ListConstIterator a, ListConstIterator b) noexcept {
		return a.m_node != b.m_node;
	}

private:
	ListNodeHeader<T>* m_node{};

	template<typename U> friend class List;
};

/// @brief Doubly linked list
template<typename T>
class List {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using iterator = ListIterator<T>;
	using const_iterator = ListConstIterator<T>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	List() noexcept:
		m_ptrs{.prev=&m_ptrs, .next=&m_ptrs},
		m_size{0}
	{ }

	template<typename InputIt>
	List(InputIt first, InputIt last): List() {
		for (auto it = first; it != last; ++it) {
			push_back(*it);
		}
	}

	List(const List& other): List() {
		for (const T& value : other) {
			push_back(value);
		}
	}

	List(List&& other) noexcept:
		m_ptrs{other.empty() ? ListNodeHeader<T>{&m_ptrs, &m_ptrs} : other.m_ptrs},
		m_size{other.m_size}
	{
		if (!other.empty()) {
			// Fix the pointer to sentinel node
			tail()->next = &end_node();
		}
		other.m_ptrs = {&other.m_ptrs, &other.m_ptrs};
		other.m_size = 0;
	}

	List& operator=(const List& other) {
		// TODO: Optimize this algorithm by reusing the memory previously allocated for old elements
		clear();
		for (const T& value : other) {
			push_back(value);
		}
		return *this;
	}

	List& operator=(List&& other) noexcept {
		clear();
		if (!other.empty()) {
			m_ptrs = other.m_ptrs;
			// Fix the pointer to sentinel node
			tail()->next = &end_node();
		}
		m_size = other.m_size;

		other.m_ptrs = {&other.end_node(), &other.end_node()};
		other.m_size = 0;
		return *this;
	}

	~List() {
		auto* node = m_ptrs.next;
		while (node != &end_node()) {
			auto* next = node->next;
			delete static_cast<ListNode<T>*>(node);
			node = next;
		}
	}

	// Iterators for direct order

	iterator begin() noexcept { return iterator{head()}; }
	const_iterator begin() const noexcept { return const_iterator{head()}; }
	const_iterator cbegin() const noexcept { return const_iterator{head()}; }

	iterator end() noexcept { return iterator{&end_node()}; }
	const_iterator end() const noexcept { return const_iterator{&end_node()}; }
	const_iterator cend() const noexcept { return const_iterator{&end_node()}; }

	// Iterators for reverse order

	reverse_iterator rbegin() noexcept { return {begin()}; }
	const_reverse_iterator rbegin() const noexcept { return {begin()}; }
	const_reverse_iterator crbegin() const noexcept { return {begin()}; }

	reverse_iterator rend() noexcept { return {end()}; }
	const_reverse_iterator rend() const noexcept { return {end()}; }
	const_reverse_iterator crend() const noexcept { return {end()}; }

	// capacity
	bool empty() const noexcept { return m_size == 0; }
	std::size_t size() const noexcept { return m_size; }

	// element access

	// modifiers
	void push_front(const T& value) {
		insert(begin(), value);
	}

	void pop_front() {
		assert(!empty());
		erase(begin());
	}

	void push_back(const T& value) {
		insert(end(), value);
	}

	void pop_back() {
		assert(!empty());
		erase(--end());
	}

	iterator insert(const_iterator pos, const T& value) {
		ListNode<T>* new_node = new ListNode<T>{ // may trhow
			{.prev=pos.m_node->prev, .next=pos.m_node},
			value
		};

		if (pos.m_node == &end_node()) {
			tail()->next = new_node;
			tail() = new_node;
		}
		else {
			if (pos.m_node->prev != &end_node()) {
				pos.m_node->prev->next = new_node;
			}
		}
		pos.m_node->prev = new_node;

		if (pos.m_node == head()) {
			head() = new_node;
		}

		++m_size;
		return iterator{new_node};
	}

	iterator erase(const_iterator pos) noexcept {
		assert(pos.m_node != &end_node());

		iterator after{pos.m_node->next};
		if (pos.m_node->prev != &end_node()) {
			pos.m_node->prev->next = pos.m_node->next;
		}
		else if (pos.m_node == head()) {
			head() = pos.m_node->next;
		}
		else {
			assert(false);
		}

		if (pos.m_node->next != &end_node()) {
			pos.m_node->next->prev = pos.m_node->prev;
		}
		else if (pos.m_node == tail()) {
			tail() = pos.m_node->prev;
		}
		else {
			assert(false);
		}

		delete pos.m_node;
		--m_size;
		return after;
	}

	iterator erase(const_iterator first, const_iterator last) noexcept {
		for (auto it = first; it != last;) {
			it = erase(it);
		}
		return iterator{last.m_node};
	}

	void swap(List& other) noexcept {
		using std::swap;

		swap(m_ptrs, other.m_ptrs);
		swap(m_size, other.m_size);

		if (empty()) {
			m_ptrs = {.prev=&end_node(), .next=&end_node};
		}
		else {
			tail()->next = &end_node();
		}

		if (other.empty()) {
			other.m_ptrs = {.prev=&other.end_node(), .next=&other.end_node};
		}
		else {
			other.tail()->next = &other.end_node();
		}
	}

	void clear() noexcept {
		for (auto* node = head(); node != &end_node();) {
			auto* next = node->next;
			delete static_cast<ListNode<T>*>(node);
			node = next;
		}
		m_ptrs = {.prev=&end_node(), .next=&end_node()};
		m_size = 0;
	}

	friend bool operator==(const List& a, const List& b) {
		if (a.size() != b.size()) {
			return false;
		}

		for (auto a_it = a.begin(), b_it = b.begin(); a_it != a.end(); ++a_it, ++b_it) {
			if (*a_it != *b_it) {
				return false;
			}
		}

		return true;
	}

	friend bool operator!=(const List& a, const List& b) {
		return !(a == b);
	}

private:
	mutable ListNodeHeader<T> m_ptrs{};
	std::size_t m_size{};

	// head is next, tail is prev
	// Helper functions

	ListNodeHeader<T>*& head() { return m_ptrs.next; }
	ListNodeHeader<T>* head() const { return m_ptrs.next; }

	ListNodeHeader<T>*& tail() { return m_ptrs.prev; }
	ListNodeHeader<T>* tail() const { return m_ptrs.prev; }

	ListNodeHeader<T>& end_node() const { return m_ptrs; }
};

template<typename T>
inline void swap(List<T>& a, List<T>& b) noexcept {
	a.swap(b);
}

#endif
