#pragma once

#include <concepts>
#include <type_traits>

namespace util::templateconstraints
{
    template <typename T> concept Arithmetic = std::is_arithmetic_v<T>;
    template <typename T> concept Pointer = std::is_pointer_v<T>;
    template <typename T> concept AnyNumeric = Arithmetic<T> || Pointer<T>;
}
