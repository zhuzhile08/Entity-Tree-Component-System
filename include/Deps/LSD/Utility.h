/*************************
 * @file Utility.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Miscelaneous utility functions
 * 
 * @date 2024-08-23
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#ifndef USE_STANDARD_LIBRARY
static_assert(false, "Cannot use replacement headers when the LSD library is available!");
#endif

#include <utility>

namespace lsd {

namespace detail {

// prime number utility

template <class Integer> inline constexpr bool isPrime(Integer n) noexcept requires std::is_integral_v<Integer> {
	if (n == 2 || n == 3)
		return true;
	else if (n <= 1 || n % 2 == 0 || n % 3 == 0)
		return false;
	else for (Integer i = 5; i * i <= n; i += 6)
		if (n % i == 0 || n % (i + 2) == 0)
			return false;
	return true;
};

template <class Integer> inline constexpr Integer nextPrime(Integer n) noexcept requires std::is_integral_v<Integer> {
	if (n % 2 == 0)
		--n;

	while (true) {
		n += 2;
		if (isPrime(n)) {
			return n;
		}
	}
};

template <class Integer> inline constexpr Integer lastPrime(Integer n) noexcept requires std::is_integral_v<Integer> {
	if (n % 2 == 0)
		++n;

	while (true) {
		n -= 2;
		if (isPrime(n)) {
			return n;
		}
	}
};


// hash map utility

inline constexpr std::size_t hashmapBucketSizeCheck(std::size_t requested, std::size_t required) noexcept {
	return (requested < required) ? nextPrime(required) : nextPrime(requested);
}

template <class Integer> inline constexpr Integer sizeToIndex(Integer size) noexcept requires std::is_integral_v<Integer> {
	return (size == 0) ? 0 : size - 1;
}

} // namespace detail


// compile time type id generator

using type_id = const void*;

template <class> struct TypeId {
private:
	constexpr static char m_id { };

	template <class> friend constexpr type_id typeId() noexcept;
};

template <class Ty> constexpr type_id typeId() noexcept {
	return &TypeId<Ty>::m_id;
}


// iterator type trait

template <class, class = void> struct IsIterator : public std::false_type { };
template <class Ty> struct IsIterator<Ty, std::void_t<
	typename std::iterator_traits<Ty>::difference_type,
	typename std::iterator_traits<Ty>::pointer,
	typename std::iterator_traits<Ty>::reference,
	typename std::iterator_traits<Ty>::value_type,
	typename std::iterator_traits<Ty>::iterator_category
>> : public std::true_type { };

template <class Ty> inline constexpr bool isIteratorValue = IsIterator<Ty>::value;

} // namespace lsd
