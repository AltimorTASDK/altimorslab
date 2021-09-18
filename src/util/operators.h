#pragma once

#include <functional>

namespace operators {

// Aliases for the generic versions of the STL operator functors
constexpr auto add = std::plus<>();
constexpr auto sub = std::minus<>();
constexpr auto mul = std::multiplies<>();
constexpr auto div = std::divides<>();
constexpr auto mod = std::modulus<>();
constexpr auto neg = std::negate<>();

// Assignment operators
constexpr auto eq = [](auto &a, auto &&b) { return a = b; };
constexpr auto add_eq = [](auto &a, auto &&b) { return a += b; };
constexpr auto sub_eq = [](auto &a, auto &&b) { return a -= b; };
constexpr auto mul_eq = [](auto &a, auto &&b) { return a *= b; };
constexpr auto div_eq = [](auto &a, auto &&b) { return a /= b; };
constexpr auto mod_eq = [](auto &a, auto &&b) { return a %= b; };

}