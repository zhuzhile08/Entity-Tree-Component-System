/*************************
 * @file Core.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Core utilities
 * 
 * @date 2024-08-23
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#ifdef USE_STANDARD_LIBRARY

#define CUSTOM_HASHER(name, type, hashType, hasher, toHashType)\
class name {\
public:\
	constexpr std::size_t operator()(type ty) const noexcept {\
		return hasher((ty)toHashType);\
	}\
	constexpr std::size_t operator()(hashType hash) const noexcept {\
		return hasher(hash);\
	}\
};

#define CUSTOM_EQUAL(name, type, hashType, toHashType)\
class name {\
public:\
	constexpr bool operator()(type first, type second) const noexcept {\
		return (first)toHashType == (second)toHashType;\
	}\
	constexpr bool operator()(type first, hashType second) const noexcept {\
		return (first)toHashType == second;\
	}\
	constexpr bool operator()(hashType first, type second) const noexcept {\
		return first == (second)toHashType;\
	}\
	constexpr bool operator()(hashType first, hashType second) const noexcept {\
		return first == second;\
	}\
};

#include <memory>
#include <vector>
#include <functional>

#else

#include <LSD/UniquePointer.h>
#include <LSD/Array.h>
#include <LSD/Vector.h>
#include <LSD/Hash.h>
#include <LSD/String.h>
#include <LSD/StringView.h>
#include <LSD/FunctionPointer.h>

#endif

#include <cstdint>

namespace etcs {

// common typedefs

using object_id = std::uint64_t;


#ifdef USE_STANDARD_LIBRARY

template <class Ty> using hash_t = std::hash<Ty>;

template <class Ty, class Deleter = std::default_delete<Ty>> using unique_ptr_t = std::unique_ptr<class Ty, class Deleter>;

template <class Ty, class... Args> using function_t = std::function<Ty(Args...)>;

template <class Ty, std::size_t Size> using array_t = std::array<Ty, Size>;

template <class Ty, class Alloc> using vector_t = std::vector<Ty, Alloc>;

template <class CharTy, class Traits = std::char_traits<CharTy>, class Alloc = std::allocator<CharTy>> using basic_string_t = std::basic_string<CharTy, Traits, Alloc>;
template <class CharTy, class Traits = std::char_traits<CharTy>> using basic_string_view_t = std::basic_string_view<CharTy, Alloc>;

#else

template <class Ty> using hash_t = lsd::Hash<Ty>;

template <class Ty, class Deleter = lsd::DefaultDelete<Ty>> using unique_ptr_t = lsd::UniquePointer<Ty, Deleter>;

template <class Ty, class... Args> using function_t = lsd::Function<Ty(Args...)>;

template <class Ty, std::size_t Size> using array_t = lsd::Array<Ty, Size>;

template <class Ty, class Alloc = std::allocator<Ty>> using vector_t = lsd::Vector<Ty, Alloc>;

template <class CharTy, class Traits = lsd::CharTraits<CharTy>, class Alloc = std::allocator<CharTy>> using basic_string_t = lsd::BasicString<CharTy, Traits, Alloc>;
template <class CharTy, class Traits = lsd::CharTraits<CharTy>> using basic_string_view_t = lsd::BasicStringView<CharTy, Traits>;

#endif

using string_t = basic_string_t<char>;
using string_view_t = basic_string_view_t<char>;


// forward declarations

class Entity;
class BasicSystem;
template <class...> class System;
class World;

#ifdef USE_COMPONENTS_EXT

class Transform;

#endif

namespace detail {

class Archetype;
class ArchetypeManager;
class EntityManager;
class SystemManager;

extern World* globalWorld;

} // namespace detail


// utility

inline static constexpr auto nullId = std::numeric_limits<object_id>::max();

namespace detail {

// entity view
struct EntityView {
	object_id id = nullId;
	string_view_t name = { };
};

// custom hashers and equal functions

CUSTOM_HASHER(EVHasher, const EntityView&, string_view_t, hash_t<string_view_t>{}, .name)
CUSTOM_EQUAL(EVEqual, const EntityView&, string_view_t, .name)

} // namespace detail

} // namespace etcs
