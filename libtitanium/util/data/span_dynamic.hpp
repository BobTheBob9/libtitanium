#pragma once

#include "span.hpp"
#include <libtitanium/memory/mem_core.hpp>

#include <concepts>
#include <string.h>

namespace util::data
{
    /*
     *  Simple vector-like struct, a span backed by a buffer that reallocates itself to fit the number of elements within it
     */
    template <typename T, std::unsigned_integral TSize = u32>
    struct SpanDynamic
    {
        /*
         *  Span for the array, length is the array's length, rather than the size allocated to it
         *  while it might make more sense to have this span just hold the allocated data, and have another field for the actual length
         *      it's much easier to do it this way, as it lets us easily reuse span utility functions for dynamics
         *      another option would be storing 1 span for the allocated buffer, and 1 for the in-use buffer, but that'd mean duplicating the data field
         */
        Span<T, TSize> sData;

        /*
         *  Actual allocated size of the buffer in sData, we generally allocate the first power of 2 that is >= sData.nLength
         */
        TSize nAllocatedLength;

        static TSize SizeForElements( const TSize nSize );

        template <memory::AllocationFuncs<T> TAllocator = memory::ALLOC_DEFAULT<T>> static void SetLength( SpanDynamic<T, TSize> *const pSpan, const TSize nLength );
        template <memory::AllocationFuncs<T> TAllocator = memory::ALLOC_DEFAULT<T>> static void InsertAt( SpanDynamic<T, TSize> *const pSpan, const T tElement, const TSize nIndex );
        template <memory::AllocationFuncs<T> TAllocator = memory::ALLOC_DEFAULT<T>> static void AppendTo( SpanDynamic<T, TSize> *const pSpan, const T tElement );
        template <memory::AllocationFuncs<T> TAllocator = memory::ALLOC_DEFAULT<T>> static void RemoveAt( SpanDynamic<T, TSize> *const pSpan, const TSize nIndex );
        template <memory::AllocationFuncs<T> TAllocator = memory::ALLOC_DEFAULT<T>> static void RemoveFromEnd( SpanDynamic<T, TSize> *const pSpan, const TSize nCount );
    };

    template <typename T, std::unsigned_integral TSize>
    TSize SpanDynamic<T, TSize>::SizeForElements( const TSize nSize )
    {
        return pow( 2, ceil( log( nSize ) / log( 2 ) ) );
    }

    template <typename T, std::unsigned_integral TSize>
    template <memory::AllocationFuncs<T> TAllocator>
    void SpanDynamic<T, TSize>::SetLength( SpanDynamic<T, TSize> *const pSpan, const TSize nLength )
    {
        pSpan->sData.nLength = nLength;

        const TSize nNewAllocated = SizeForElements( nLength );
        if ( pSpan->nAllocatedLength == nNewAllocated )
        {
            return;
        }

        pSpan->nAllocatedLength = nNewAllocated;

        if ( nNewAllocated )
        {
            pSpan->sData.pData = TAllocator.fnReallocateCount( pSpan->sData.pData, pSpan->nAllocatedLength, nullptr );
        }
        else
        {
            TAllocator.fnFree( pSpan->sData.pData, nullptr );
        }
    }

    template <typename T, std::unsigned_integral TSize>
    template <memory::AllocationFuncs<T> TAllocator>
    void SpanDynamic<T, TSize>::InsertAt( SpanDynamic<T, TSize> *const pSpan, const T tElement, const TSize nIndex )
    {
        // ensure we have enough space for the new element
        SetLength<TAllocator>( pSpan, pSpan->sData.nLength + 1 );

        // move what's currently at the index, and everything after it +1 spaces away, so the index we want is free, then set it
        memcpy( &pSpan->sData.pData[ nIndex + 1 ], &pSpan->sData.pData[ nIndex ], sizeof( T ) * ( pSpan->sData.nLength - nIndex ) );
        pSpan->sData.pData[ nIndex ] = tElement;
    }

    template <typename T, std::unsigned_integral TSize>
    template <memory::AllocationFuncs<T> TAllocator>
    void SpanDynamic<T, TSize>::AppendTo( SpanDynamic<T, TSize> *const pSpan, const T tElement )
    {
        // ensure we have enough space for the new element
        SetLength<TAllocator>( pSpan, pSpan->sData.nLength + 1 );
        pSpan->sData.pData[ pSpan->sData.nLength - 1 ] = tElement;
    }

    template <typename T, std::unsigned_integral TSize>
    template <memory::AllocationFuncs<T> TAllocator>
    void SpanDynamic<T, TSize>::RemoveAt( SpanDynamic<T, TSize> *const pSpan, const TSize nIndex )
    {
        // move what's currently after the index, so only the index we want to affect gets changed
        memcpy( &pSpan->sData.pData[ nIndex ], &pSpan->sData.pData[ nIndex + 1 ], sizeof( T ) * ( pSpan->sData.nLength - nIndex - 1 ) );
        SetLength<TAllocator>( pSpan, pSpan->sData.nLength - 1 );
    }

    template <typename T, std::unsigned_integral TSize>
    template <memory::AllocationFuncs<T> TAllocator>
    void SpanDynamic<T, TSize>::RemoveFromEnd( SpanDynamic<T, TSize> *const pSpan, const TSize nCount )
    {
        SetLength<TAllocator>( pSpan, pSpan->sData.nLength - nCount );
    }
}
