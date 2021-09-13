#include <tuple>
#include <utility>

template<typename ...T>
constexpr auto sum_tuple(const std::tuple<T...> &tuple)
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return (std::get<I>(tuple) + ...);
	}(std::make_index_sequence<sizeof...(T)>());
}