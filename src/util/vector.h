#pragma once

#include "util/types.h"
#include "util/misc.h"
#include "util/operators.h"
#include <cmath>
#include <utility>
#include <tuple>
#include <type_traits>

template<typename base>
class vec_impl : public base {
public:
	using base::elems;

private:
	using elem_tuple = decltype(std::declval<const base>().elems());
	static constexpr auto elem_count = sizeof_tuple<elem_tuple>;
	static constexpr auto elem_indices = std::make_index_sequence<elem_count>();

	constexpr auto foreach(auto &&callable, auto &&...tuples)
	{
		static_assert(((sizeof_tuple<decltype(tuples)> == elem_count) && ...));
		return zip_apply(callable, elems(), tuples...);
	}

	constexpr auto foreach(auto &&callable, auto &&...tuples) const
	{
		static_assert(((sizeof_tuple<decltype(tuples)> == elem_count) && ...));
		return zip_apply(callable, elems(), tuples...);
	}

public:
	constexpr vec_impl()
	{
		foreach(bind_back(operators::eq, 0));
	}

	template<typename ...T, typename = std::enable_if_t<sizeof...(T) == elem_count>>
	constexpr vec_impl(T ...values)
	{
		elems() = std::make_tuple(values...);
	}

	explicit constexpr vec_impl(elem_tuple &&tuple)
	{
		elems() = tuple;
	}

	constexpr vec_impl(const vec_impl &other)
	{
		*this = other;
	}

	template<size_t N>
	constexpr auto &get() { return std::get<N>(elems()); }

	template<size_t N>
	constexpr auto get() const { return std::get<N>(elems()); }

	template<typename... T>
	constexpr vec_impl &operator=(const vec_impl &other)
	{
		elems() = other.elems();
		return *this;
	}

	template<typename... T>
	constexpr vec_impl &operator+=(const vec_impl &other)
	{
		foreach(operators::add_eq, other.elems());
		return *this;
	}

	template<typename... T>
	constexpr vec_impl operator+(const vec_impl &other) const
	{
		return vec_impl(foreach(operators::add, other.elems()));
	}

	template<typename... T>
	constexpr vec_impl &operator-=(const vec_impl &other)
	{
		foreach(operators::sub_eq, other.elems());
		return *this;
	}

	template<typename... T>
	constexpr vec_impl operator-(const vec_impl &other) const
	{
		return vec_impl(foreach(operators::sub, other.elems()));
	}

	template<typename... T>
	constexpr vec_impl &operator*=(auto value)
	{
		foreach(bind_back(operators::mul_eq, value));
		return *this;
	}

	template<typename... T>
	constexpr vec_impl operator*(auto value) const
	{
		return vec_impl(foreach(bind_back(operators::mul, value)));
	}

	template<typename... T>
	constexpr vec_impl &operator/=(auto value)
	{
		foreach(bind_back(operators::div_eq, value));
		return *this;
	}

	template<typename... T>
	constexpr vec_impl operator/(auto value) const
	{
		return vec_impl(foreach(bind_back(operators::div, value)));
	}

	template<typename... T>
	constexpr bool operator==(const vec_impl &other) const
	{
		return elems() == other.elems();
	}

	constexpr auto length_sqr() const
	{
		return dot(*this, *this);
	}

	constexpr auto length() const
	{
		return sqrt(length_sqr());
	}

	static constexpr auto dot(const vec_impl &a, const vec_impl &b)
	{
		return sum_tuple(zip_apply(operators::mul, a.elems(), b.elems()));
	}
};

template<typename T>
struct vec2_base {
	T x, y;
	constexpr auto elems() { return std::tie(x, y); }
	constexpr auto elems() const { return std::make_tuple(x, y); }
};

using vec2 = vec_impl<vec2_base<float>>;
using vec2i = vec_impl<vec2_base<s32>>;
using vec2d = vec_impl<vec2_base<double>>;

template<typename T>
struct vec3_base {
	T x, y, z;
	constexpr auto elems() { return std::tie(x, y, z); }
	constexpr auto elems() const { return std::make_tuple(x, y, z); }

	static constexpr vec_impl<vec3_base> cross(const auto &a, const auto &b)
	{
		return vec_impl<vec3_base>(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
	}
};

using vec3 = vec_impl<vec3_base<float>>;
using vec3i = vec_impl<vec3_base<s32>>;
using vec3d = vec_impl<vec3_base<double>>;

template<typename T>
struct color_rgb_base {
	T r, g, b;
	constexpr auto elems() { return std::tie(r, g, b); }
	constexpr auto elems() const { return std::make_tuple(r, g, b); }
};

using color_rgb = vec_impl<color_rgb_base<u8>>;
using color_rgb_f32 = vec_impl<color_rgb_base<float>>;

template<typename T>
struct color_rgba_base {
	T r, g, b, a;
	constexpr auto elems() { return std::tie(r, g, b, a); }
	constexpr auto elems() const { return std::make_tuple(r, g, b, a); }
};

using color_rgba = vec_impl<color_rgba_base<u8>>;
using color_rgba_f32 = vec_impl<color_rgba_base<float>>;