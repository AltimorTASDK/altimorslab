#include "util/int_types.h"
#include <functional>
#include <utility>
#include <tuple>
#include <type_traits>

template<typename base>
class vec_impl : public base {
private:
	static constexpr size_t elem_count =
		std::tuple_size_v<decltype(std::declval<vec_impl>().elems())>;

	static constexpr auto elem_indices = std::make_index_sequence<elem_count>();

	constexpr auto foreach(const auto &callable)
	{
		if constexpr(std::is_same_v<decltype(callable(get<0>())), void>) {
			[&]<size_t ...I>(std::index_sequence<I...>) {
				(callable(this->get<I>()), ...);
			}(elem_indices);
		} else {
			return [&]<size_t ...I>(std::index_sequence<I...>) {
				return std::make_tuple(callable(this->get<I>())...);
			}(elem_indices);
		}
	}

	constexpr auto foreach(const auto &callable, auto ...args)
	{
		static_assert(sizeof...(args) == elem_count);

		if constexpr ((std::is_same_v<decltype(callable(get<0>(), args)), void> || ...)) {
			[&]<size_t ...I>(std::index_sequence<I...>) {
				(callable(this->get<I>(), args), ...);
			}(elem_indices);
		} else {
			return [&]<size_t ...I>(std::index_sequence<I...>) {
				return std::make_tuple(callable(this->get<I>(), args)...);
			}(elem_indices);
		}
	}

	constexpr auto foreach(const auto &callable, const auto &tuple)
	{
		return std::apply([&](auto ...args) { return foreach(callable, args...); }, tuple);
	}

	constexpr auto foreach(const auto &callable, const vec_impl &other)
	{
		return foreach(callable, other.to_tuple());
	}

public:
	constexpr vec_impl()
	{
		foreach([](auto &elem) { elem = 0; });
	}

	constexpr vec_impl(auto ...values)
	{
		foreach([](auto &elem, auto value) { elem = value; }, values...);
	}

	constexpr vec_impl(const vec_impl &other)
	{
		*this = other;
	}

	template<size_t N>
	constexpr auto &get() { return std::get<N>(base::elems()); }

	template<size_t N>
	constexpr auto get() const { return std::get<N>(base::elems()); }

	constexpr auto to_tuple() const
	{
		return [&]<size_t ...I>(std::index_sequence<I...>) {
			return std::make_tuple(this->get<I>()...);
		}(elem_indices);
	}

	template<typename... T>
	constexpr vec_impl &operator=(const vec_impl &other)
	{
		foreach([](auto &elem, auto value) { elem = value; }, other);
		return *this;
	}

	template<typename... T>
	constexpr vec_impl &operator+=(const vec_impl &other)
	{
		foreach([](auto &elem, auto value) { elem += value; }, other);
		return *this;
	}

	template<typename... T>
	constexpr vec_impl &operator-=(const vec_impl &other)
	{
		foreach([](auto &elem, auto value) { elem -= value; }, other);
		return *this;
	}

	template<typename... T>
	constexpr vec_impl &operator*=(auto value)
	{
		foreach([&](auto &elem) { elem *= value; });
		return *this;
	}

	template<typename... T>
	constexpr vec_impl &operator/=(auto value)
	{
		foreach([&](auto &elem) { elem /= value; });
		return *this;
	}

	template<typename... T>
	constexpr bool operator==(const vec_impl &other)
	{
		return to_tuple() == other.to_tuple();
	}

	static constexpr auto dot(const vec_impl &a, const vec_impl &b)
	{
		return [&]<size_t ...I>(std::index_sequence<I...>) {
			return ((a.get<I>() * b.get<I>()) + ...);
		}(elem_indices);
	}
};

template<typename T>
struct vec2_base {
	float x, y;
protected:
	constexpr auto elems() { return std::tie(x, y); }
	constexpr auto elems() const { return std::make_tuple(x, y); }
};

using vec2 = vec_impl<vec2_base<float>>;
using vec2i = vec_impl<vec2_base<s32>>;
using vec2d = vec_impl<vec2_base<double>>;

template<typename T>
struct vec3_base {
	T x, y, z;
protected:
	constexpr auto elems() { return std::tie(x, y, z); }
	constexpr auto elems() const { return std::make_tuple(x, y, z); }

public:
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
protected:
	constexpr auto elems() { return std::tie(r, g, b); }
	constexpr auto elems() const { return std::make_tuple(r, g, b); }
};

using color_rgb = vec_impl<color_rgb_base<u8>>;
using color_rgb_f32 = vec_impl<color_rgb_base<float>>;

template<typename T>
struct color_rgba_base {
	T r, g, b, a;
protected:
	constexpr auto elems() { return std::tie(r, g, b, a); }
	constexpr auto elems() const { return std::make_tuple(r, g, b, a); }
};

using color_rgba = vec_impl<color_rgba_base<u8>>;
using color_rgba_f32 = vec_impl<color_rgba_base<float>>;