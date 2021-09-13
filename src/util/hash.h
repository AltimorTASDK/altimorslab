#pragma once

#include "util/int_types.h"
#include <utility>
#include <cstddef>

constexpr u32 strhash_impl(u32 hash, char head)
{
	// Skip null terminator
	return hash;
}

template<typename ...T>
constexpr u32 strhash_impl(u32 hash, char head, T ...tail)
{
	// Recursively hash each char
	constexpr auto fnv_prime = 0x01000193;
	return strhash_impl((hash ^ head) * fnv_prime, tail...);
}

template<size_t... indices>
constexpr u32 strhash_impl(const char str[], std::index_sequence<indices...>)
{
	constexpr auto fnv_offset_basis = 0x811C9DC5;
	// Explode array
	return strhash_impl(fnv_offset_basis, str[indices]...);
}

template<size_t N, typename indices = std::make_index_sequence<N>>
constexpr u32 strhash(const char (&str)[N])
{
	return strhash_impl(str, indices{});
}