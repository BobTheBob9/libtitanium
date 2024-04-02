#pragma once

#include "span_dynamic.hpp"

namespace util::data
{
    template <typename T> struct MultiTree
    {
        T data;
        SpanDynamic<MultiTree<T>> childNodes;
        MultiTree<T> * parentNode;
    };
}
