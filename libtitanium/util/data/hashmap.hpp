#pragma once

#include <concepts>

#include <libtitanium/util/numerics.hpp>

namespace util::data
{
    template <typename TValue, std::unsigned_integral TSize = u64>
    class HashMap
    {
    private:
        

    public:
        void Insert( const TSize nHash, TValue tValue );
        void Remove( const TSize nHash );
        TValue Get( const TSize nHash );
    };
}

#include "hashmap_impl.inl"
