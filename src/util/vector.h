#include "util/meta.h"
#include <functional>
#include <utility>
#include <tuple>
#include <type_traits>

template<typename base>
class vec_impl : public base {
private:
	template<size_t N>
	static constexpr auto index = std::integral_constant<size_t, N>();

	template<size_t N>
	constexpr auto &get() { return base::get_impl(index<N>); }

	template<size_t N>
	constexpr auto get() const { return base::get_impl(index<N>); }

	template<size_t N, typename enable = void>
	struct elem_exists { static constexpr auto value = false; };

	template<size_t N>
	struct elem_exists<N, std::void_t<decltype(std::declval<vec_impl>().get_impl(index<N>))>> {
		static constexpr auto value = true;
	};

	template<size_t N = 0>
	static constexpr size_t elem_count()
	{
		if constexpr (elem_exists<N>::value)
			return elem_count<N + 1>();
		else
			return N;
	}

	static constexpr auto elem_indices = std::make_index_sequence<elem_count()>();

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
		static_assert(sizeof...(args) == elem_count());

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
	constexpr vec_impl &operator*=(float value)
	{
		foreach([&](auto &elem) { elem *= value; });
		return *this;
	}

	constexpr auto dot(const vec_impl &other)
	{
		/*return sum_tuple(
			foreach([](auto &elem, auto value) { return elem * value; }, other));*/

		return [&]<size_t ...I>(std::index_sequence<I...>) {
			return ((this->get<I>() * other.get<I>()) + ...);
		}(elem_indices);
	}
};

struct vec3_base {
	float x, y, z;
protected:
	constexpr auto &get_impl(std::integral_constant<size_t, 0>) { return x; }
	constexpr auto &get_impl(std::integral_constant<size_t, 1>) { return y; }
	constexpr auto &get_impl(std::integral_constant<size_t, 2>) { return z; }
	constexpr auto get_impl(std::integral_constant<size_t, 0>) const { return x; }
	constexpr auto get_impl(std::integral_constant<size_t, 1>) const { return y; }
	constexpr auto get_impl(std::integral_constant<size_t, 2>) const { return z; }
};

using vec3 = vec_impl<vec3_base>;