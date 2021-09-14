#pragma once

#include <algorithm>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

template<typename T>
constexpr auto is_void = std::is_same_v<T, void>;

// Like std::tuple_size_v, but accepts reference types
template<typename T>
constexpr auto sizeof_tuple = std::tuple_size_v<std::remove_reference_t<T>>;

constexpr auto align(auto value, auto alignment)
{
	// Powers of 2 only
	return (value + alignment - 1) & ~(alignment - 1);
}

constexpr auto sum_tuple(auto &&tuple)
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return (std::get<I>(tuple) + ...);
	}(std::make_index_sequence<sizeof_tuple<decltype(tuple)>>());
}

// Replaces empty tuples with void.
constexpr auto tuple_or_void(auto &&tuple)
{
	if constexpr (sizeof_tuple<decltype(tuple)> != 0)
		return std::move(tuple);
}

// Like std::bind_front
constexpr auto bind_back(auto &&callable, auto &&...args)
{
	return [&](auto &&...head) {
		return callable(
			std::forward<decltype(head)>(head)...,
			std::forward<decltype(args)>(args)...);
	};
}

template<typename ...T, size_t start, size_t end = sizeof...(T)>
constexpr auto slice_tuple(const std::tuple<T...> &tuple)
{
	// Python style indices
	constexpr auto size = sizeof...(T);

	constexpr auto real_start = start >= 0
		? std::min(start, size - 1)
		: std::max((size_t)0, size + start);

	constexpr auto real_end = end >= 0
		? std::min(end, size)
		: std::max((size_t)0, size + end);

	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return std::forward_as_tuple(std::get<real_start + I>(tuple)...);
	}(std::make_index_sequence<real_end - real_start>());
}

template<typename ...T, size_t start, size_t end = sizeof...(T)>
constexpr auto slice(T &&...args)
{
	return slice_tuple<T..., start, end>(std::forward_as_tuple(args...));
}

// Create a tuple of the Nth elements of the given tuples. If a tuple has no Nth
// element, it's omitted from the result.
template<size_t N>
constexpr auto zip_at_index(auto &&...tuples)
{
	return std::tuple_cat([&]() {
		if constexpr (N < sizeof_tuple<decltype(tuples)>)
			return std::forward_as_tuple(std::get<N>(tuples));
		else
			return std::make_tuple();
	}()...);
}

// Similar to Python's itertools.zip_longest, but without a fill value.
constexpr auto zip(auto &&...tuples)
{
	if constexpr (sizeof...(tuples) > 1) {
		constexpr auto longest = std::max(sizeof_tuple<decltype(tuples)>...);

		return [&]<size_t ...I>(std::index_sequence<I...>) {
			return std::make_tuple(zip_at_index<I>(tuples...)...);
		}(std::make_index_sequence<longest>());
	} else {
		// If only one tuple, wrap each element in a tuple
		constexpr auto size = sizeof_tuple<decltype(tuples)...>;

		return [&]<size_t ...I>(std::index_sequence<I...>) {
			return std::make_tuple(std::forward_as_tuple(std::get<I>(tuples...))...);
		}(std::make_index_sequence<size>());
	}
}

// Use std::apply with multiple argument tuples, returning a tuple of results.
// void results are omitted from the tuple. If no results are returned,
// this function returns void.
constexpr auto apply_multi(auto &&callable, auto &&...tuples)
{
	return tuple_or_void(std::tuple_cat([&]() {
		if constexpr (!is_void<decltype(std::apply(callable, tuples))>) {
			return std::make_tuple(std::apply(callable, tuples));
		} else {
			std::apply(callable, tuples);
			return std::make_tuple();
		}
	}()...));
}

// Given tuple A of length 3, tuple B of length 2, and tuple C of length 4,
// zip_apply is equivalent to the following:
//
// callable(std::get<0>(A), std::get<0>(B), std::get<0>(C))
// callable(std::get<1>(A), std::get<1>(B), std::get<1>(C))
// callable(std::get<2>(A), std::get<2>(C))
// callable(std::get<3>(C))
//
// The results of callable are returned as a tuple. void results are omitted
// from the tuple. If no results are returned, this function returns void.
constexpr auto zip_apply(auto &&callable, auto &&...tuples)
{
	return std::apply([&](auto &&...args) {
		return apply_multi(callable, std::forward<decltype(args)>(args)...);
	}, zip(tuples...));
}