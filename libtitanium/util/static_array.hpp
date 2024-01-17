#pragma once

#include <libtitanium/util/data/span.hpp>

namespace util
{
    template <typename T, size_t N>
    constexpr uint StaticArray_Length( T( & )[ N ] ) { return N; }
    template <typename T, size_t N>
    util::data::Span<T> StaticArray_ToSpan( T( &tArr )[ N ] ) { return { .nLength = N, .pData = tArr }; }
}
