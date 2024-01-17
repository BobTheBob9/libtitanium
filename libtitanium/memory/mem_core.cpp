#include "mem_core.hpp"

#include <libtitanium/util/assert.hpp>
#include <libtitanium/util/static_array.hpp>
#include <libtitanium/util/stacktrace.hpp>
#include <libtitanium/util/data/span_dynamic.hpp>
#include <libtitanium/logger/logger.hpp>

#include <stdlib.h>
#include <atomic>
#include <vector>

// TODO: most of this is pretty temp, needs to be like at all fucking threadsafe and cleaner
// overall, just cleanup this alot, it sucks but does work

namespace memory
{
#if HAS_MEM_DEBUG
    struct AllocInfo
    {
        void * pAddress;
        util::StackMember stacktrace[ 64 ] {};
    };

    // TODO: TEMP!!! very distinct lack of thread safety here
    static std::atomic<size_t> s_nAllocs = 0;
    static util::data::SpanDynamic<AllocInfo> s_vAllocations {};
#endif // #if HAS_MEM_DEBUG

    void* alloc( const size_t nBytes )
    {
        assert::Debug( nBytes );

        void * r_pData = ::malloc( nBytes );

#if HAS_MEM_DEBUG
        s_nAllocs++;

        AllocInfo alloc { .pAddress = r_pData };
        util::GetStacktrace( util::StaticArray_ToSpan( alloc.stacktrace ) );
        util::data::SpanDynamic<AllocInfo>::AppendTo<memory::ALLOC_UNTRACKED<AllocInfo>>( &s_vAllocations, alloc );
#endif // #if HAS_MEM_DEBUG

        return r_pData;
    }

    void* realloc( void *const pMemoryToRealloc, const size_t nNewSize )
    {
        assert::Debug( pMemoryToRealloc, "pMemoryToRealloc is null" );

        void * r_pReallocated = ::realloc( pMemoryToRealloc, nNewSize );

#if HAS_MEM_DEBUG
        bool bFound = false;
        for ( uint i = 0; i < s_vAllocations.sData.nLength; i++ )
        {
            if ( pMemoryToRealloc == s_vAllocations.sData.pData[ i ].pAddress )
            {
                assert::Debug( !bFound, "!bFound" );
                s_vAllocations.sData.pData[ i ].pAddress = r_pReallocated;
                bFound = true;
            }
        }
        assert::Debug( bFound, "bFound" );

#endif // #if HAS_MEM_DEBUG

        return r_pReallocated;
    }

    void free( void* pMemoryToFree )
    {
        assert::Debug( pMemoryToFree );

#if HAS_MEM_DEBUG
        s_nAllocs--;

        bool bFound = false;
        for ( uint i = 0; i < s_vAllocations.sData.nLength; i++ )
        {
            if ( pMemoryToFree == s_vAllocations.sData.pData[ i ].pAddress )
            {
                util::data::SpanDynamic<AllocInfo>::RemoveAt<memory::ALLOC_UNTRACKED<AllocInfo>>( &s_vAllocations, i );
                bFound = true;
                break;
            }
        }

        for ( uint i = 0; i < s_vAllocations.sData.nLength; i++ )
        {
            assert::Debug( pMemoryToFree != s_vAllocations.sData.pData[ i ].pAddress );
        }

        assert::Debug( bFound, "bFound" );
#endif // #if HAS_MEM_DEBUG

        ::free( pMemoryToFree );
    }

    void * externalMalloc( size_t size ) { return ::memory::alloc( size ); }
    void * externalCalloc( size_t nmemb, size_t size )
    {
        void * r_pData = ::calloc( nmemb, size );

#if HAS_MEM_DEBUG
        // TODO: mempool and logging logic
        s_nAllocs++;

        AllocInfo alloc { .pAddress = r_pData };
        util::GetStacktrace( util::StaticArray_ToSpan( alloc.stacktrace ) );
        util::data::SpanDynamic<AllocInfo>::AppendTo<memory::ALLOC_UNTRACKED<AllocInfo>>( &s_vAllocations, alloc );
#endif // #if HAS_MEM_DEBUG
        return r_pData;
    }

    void * externalRealloc( void * ptr, size_t size )
    {
        if ( !ptr )
        {
            return alloc( size );
        }
        else
        {
            return realloc( ptr, size );
        }
    }

    void * externalReallocarray( void * ptr, size_t nmemb, size_t size ) { return ::reallocarray( ptr, nmemb, size ); }
    void externalFree( void * ptr ) { if ( ptr ) return ::memory::free( ptr ); }

    size_t GetAllocs()
    {
#if HAS_MEM_DEBUG
        return s_nAllocs;
#else // #if HAS_MEM_DEBUG
        return 0;
#endif // #else // #if HAS_MEM_DEBUG
    }

    void ReportAllocationInfo()
    {
#if HAS_MEM_DEBUG
        assert::Debug( (u64)s_nAllocs == s_vAllocations.sData.nLength );

        if ( !s_nAllocs )
        {
            logger::Info( "No leaked allocations! Good job!" ENDL );
        }

        for ( uint i = 0; i < s_vAllocations.sData.nLength; i++ )
        {
            AllocInfo alloc = s_vAllocations.sData.pData[ i ];
            logger::Info( "unfreed alloc at %p, originating from:" ENDL, alloc.pAddress );

            for ( uint j = 0; j < util::StaticArray_Length( alloc.stacktrace ); j++ )
            {
                if ( !*alloc.stacktrace[ j ].szFuncName )
                {
                    break;
                }

                logger::Info( "\t%s" ENDL, alloc.stacktrace[ j ].szFuncName );
            }
        }
#endif // #if HAS_MEM_DEBUG
    }
}
