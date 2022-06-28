#ifndef SCOCPED_PTR_HPP_INCLUDED
#define SCOCPED_PTR_HPP_INCLUDED

#include <utility>
#include <concepts>

/// @brief Smart pointer that owns a dynamically allocated object.
/// The object is disposed of using `delete` when the `ScopedPtr` goes out of scope.
///
/// ScopedPtr is a simplified version of `std::unique_ptr`
template<typename T>
class ScopedPtr {
public:
	ScopedPtr() noexcept = default;
	ScopedPtr(T* ptr) noexcept:
		m_ptr(ptr)
	{ }

	// Copying is banned

	ScopedPtr(const ScopedPtr&) = delete;
	ScopedPtr& operator=(const ScopedPtr&) = delete;

	// Moving transfers ownership from one ScopedPtr to another

	/// @brief Construct a `ScopedPtr` by transfering ownership from another `ScopedPtr` of
	/// the same type
	ScopedPtr(ScopedPtr&& other) noexcept:
		m_ptr(other.m_ptr)
	{
		other.release();
	}

	/// @brief Transfer owenship from `other` to `*this`. Object previously managed by `*this` will
	/// be deleted
	ScopedPtr& operator=(ScopedPtr&& other) noexcept {
		reset(other.release());
	}

	/// @brief Construct a `ScopedPtr` by transfering ownership from another `ScopedPtr` of
	/// a different type
	/// @note This constructor is only available if `U*` is convertible to `T*`
	template<typename U>
		requires std::convertible_to<U*, T*>
	ScopedPtr(ScopedPtr<U>&& other) noexcept:
		m_ptr(other.get())
	{
		other.release();
	}

	~ScopedPtr() noexcept {
		delete m_ptr;
	}

	// Getters

	T* get() const noexcept { return m_ptr; }
	T& operator*() const noexcept { return *m_ptr; }
	T* operator->() const noexcept { return m_ptr; }
	operator bool() const noexcept { return m_ptr; }

	/// @brief Release the ownership of the managed object
	T* release() noexcept {
		T* old_ptr = m_ptr;
		m_ptr = nullptr;
		return old_ptr;
	}

	/// @brief Replace the managed object
	void reset(T* new_ptr = nullptr) noexcept {
		T* old_ptr = m_ptr;
		m_ptr = new_ptr;
		delete old_ptr;
	}

	void swap(ScopedPtr& other) {
		using std::swap;

		swap(m_ptr, other.m_ptr);
	}

private:
	T* m_ptr = nullptr;
};

template<typename T>
inline void swap(ScopedPtr<T>& a, ScopedPtr<T>& b) noexcept {
	a.swap(b);
}

#endif
