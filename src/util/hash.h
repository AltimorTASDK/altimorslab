#pragma once

#include "util/int_types.h"

template<size_t N, size_t index = 0, u32 offset_basis = 0x811C9DC5, u32 prime = 0x01000193>
constexpr u32 strhash(const char (&str)[N], u32 hash = offset_basis)
{
	// Recursively hash each char, excluding null terminator
	if constexpr (index < N - 1)
		return strhash<N, index + 1>(str, (hash ^ str[index]) * prime);
	else
		return hash;
}