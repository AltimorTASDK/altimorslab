#include <functional>

// Aliases for the generic versions of the STL operator functors

namespace operators {

constexpr auto add = std::plus<>();
constexpr auto sub = std::minus<>();
constexpr auto mul = std::multiplies<>();
constexpr auto div = std::divides<>();
constexpr auto mod = std::modulus<>();
constexpr auto neg = std::negate<>();

}