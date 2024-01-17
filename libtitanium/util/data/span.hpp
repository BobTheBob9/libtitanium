#pragma once

#include <libtitanium/memory/mem_core.hpp>
#include <libtitanium/util/numerics.hpp>

#include <concepts>

namespace util::data
{
    /*
     *  A typed pointer with a length. Super general purpose
     */
    template <typename T, std::unsigned_integral TSize = u32>
    struct Span
    {
        TSize nLength;
        T * pData;

        static Span<T, TSize> Offset( const Span<T, TSize> *const pSpan, const TSize nOffset );
        static Span<T, TSize> Slice( const Span<T, TSize> *const pSpan, const TSize nFirstIndex, const TSize nLength );

        using FnFindCompare = bool(*)( const T *const ptCurrentValue, const T *const ptUserComparator );
        static bool ValueFind( const T *const ptCurrentValue, const T *const ptUserComparator );

        static T * Find( const Span<T, TSize> *const pSpan, const FnFindCompare fnComparator, const T *const tFindValue );

        struct R_IndexOf
        {
            bool bFound;
            TSize nIndex;
        };
        static R_IndexOf IndexOf( const Span<T, TSize> *const pSpan, const FnFindCompare fnComparator, const T *const tFindValue );

        template <memory::AllocationFuncs<T> TAllocator = memory::ALLOC_DEFAULT<T>> static void ReallocateResize( Span<T, TSize> *const pSpan, const TSize nLength );
    };  



    template <typename T, std::unsigned_integral TSize>
    Span<T, TSize> Span<T, TSize>::Offset( const Span<T, TSize> *const pSpan, const TSize nOffset )
    {
        return { .nLength = pSpan->nLength - nOffset, .pData = pSpan->pData + nOffset };
    }

    template <typename T, std::unsigned_integral TSize>
    Span<T, TSize> Span<T, TSize>::Slice( const Span<T, TSize> *const pSpan, const TSize nFirstIndex, const TSize nLength )
    {
        return { .nLength = nLength, .pData = pSpan->pData + nFirstIndex };
    }

    template <typename T, std::unsigned_integral TSize>
    bool Span<T, TSize>::ValueFind( const T *const ptCurrentValue, const T *const ptUserComparator )
    {
        return *ptCurrentValue == *ptUserComparator;
    }

    template <typename T, std::unsigned_integral TSize>
    T * Span<T, TSize>::Find( const Span<T, TSize> *const pSpan, const FnFindCompare fnComparator, const T *const ptFindValue )
    {
        for ( TSize i = 0; i < pSpan->nLength; i++ )
        {
            if ( fnComparator( &pSpan->pData[ i ], ptFindValue ) )
            {
                return &pSpan->pData[ i ];
            }
        }

        return nullptr;
    }

    template <typename T, std::unsigned_integral TSize>
    typename Span<T, TSize>::R_IndexOf Span<T, TSize>::IndexOf( const Span<T, TSize> *const pSpan, const FnFindCompare fnComparator, const T *const ptFindValue )
    {
        for ( TSize i = 0; i < pSpan->nLength; i++ )
        {
            if ( fnComparator( &pSpan->pData[ i ], ptFindValue ) )
            {
                return {
                    .bFound = true,
                    .nIndex = i
                };
            }
        }

        return {
            .bFound = false
        };
    }

    template <typename T, std::unsigned_integral TSize>
    template <memory::AllocationFuncs<T> TAllocator>
    void Span<T, TSize>::ReallocateResize( Span<T, TSize> *const pSpan, const TSize nLength )
    {
        pSpan->nLength = nLength;
        pSpan->pData = TAllocator.fnReallocateCount( pSpan->pData, nLength );
    }
}
