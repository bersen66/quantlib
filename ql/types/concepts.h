#pragma once
#include <concepts>

namespace ql {

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

}  // namespace ql