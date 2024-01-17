#pragma once

#include <libtitanium/util/debug.hpp>
#include <libtitanium/util/numerics.hpp>

#include <source_location>

/*
 *  Functions for allocating memory and tracking the allocation of memory
 */
namespace memory
{
    /*
     *  HAS_MEM_DEBUG controls whether we should compile in runtime memory debugging features
     *  We do still include a few monitoring features without HAS_MEM_DEBUG, but most (high perf cost) shouldn't be
     */
#ifndef HAS_MEM_DEBUG
    #define HAS_MEM_DEBUG HAS_DEBUG
    #define HAS_MEM_STACKTRACE HAS_MEM_DEBUG
#endif  // #ifndef MALLOC_DEBUG

    /*
     *  General-purpose allocation function, takes a non-zero number of bytes
     */
    void* alloc( const size_t nSize );

    /*
    
    Templated malloc function to avoid unnecessary casts

    */
    template<typename T>
    inline T* alloc_T( const size_t nSize )
    { return static_cast<T*>( alloc( nSize ) ); }

    /*
    
    Templated malloc function, rather than taking a number of bytes, it allocates sizeof(T) * nElements

    */
    template<typename T>
    inline T* alloc_nT( const size_t nElements )
    { return alloc_T<T>(sizeof(T) * nElements ); }

    /* 
    
    Wrapped for realloc
    
    */
    void* realloc( void *const pMemoryToRealloc, const size_t nNewSize);

    /*
    
    Templated realloc function to avoid unnecessary casts and ensure typesafety

    */
    template<typename T>
    inline T* realloc_T( T *const pMemoryToRealloc, const size_t nNewSize)
    { return static_cast<T*>(realloc(pMemoryToRealloc, nNewSize ) ); }

    /*
    
    Templated realloc function, rather than taking a number of bytes, it allocates sizeof(T) * nElements

    */
    template<typename T>
    inline T* realloc_nT( T *const pMemoryToRealloc, const size_t nElements )
    { return realloc_T<T>(pMemoryToRealloc, sizeof(T) * nElements); }

    /*
    
    Wrapper for free

    */
    void free(void* pMemoryToFree);

    void * externalMalloc( size_t size );
    void externalFree( void * ptr );
    void * externalCalloc( size_t nmemb, size_t size );
    void * externalRealloc( void * ptr, size_t size );
    void * externalReallocarray( void * ptr, size_t nmemb, size_t size );

#if HAS_MEM_DEBUG
    size_t GetAllocs();
    void ReportAllocationInfo();
#endif // #if HAS_MEM_DEBUG

    template <typename T> using FnAllocateGeneric = T*(*)( const size_t nSize, void *const pUserData );
    template <typename T> using FnAllocateCountGeneric = T*(*)( const size_t nCount, void *const pUserData );
    template <typename T> using FnReallocateGeneric = T*(*)( T *const pPtr, const size_t nNewSize, void *const pUserData );
    template <typename T> using FnReallocateCountGeneric = T*(*)( T *const pPtr, const size_t nNewCount, void *const pUserData );
    template <typename T> using FnFreeGeneric = void(*)( T *const pPtr, void *const pUserData );

    template <typename T>
    struct AllocationFuncs
    {
        FnAllocateGeneric<T> fnAllocate;
        FnAllocateCountGeneric<T> fnAllocateCount;
        FnReallocateGeneric<T> fnReallocate;
        FnReallocateCountGeneric<T> fnReallocateCount;
        FnFreeGeneric<T> fnFree;
    };

    template <typename T> T * AllocateGenericDefault( const size_t nSize, void *const pUserData ) { (void)pUserData; return alloc_T<T>( nSize ); }
    template <typename T> T * AllocateCountGenericDefault( const size_t nCount, void *const pUserData ) { (void)pUserData; return alloc_nT<T>( nCount ); }
    template <typename T> T * ReallocateGenericDefault( T * pPtr, const size_t nNewSize, void *const pUserData ) {
        (void)pUserData;

        if ( pPtr )
        {
            return realloc_T<T>( pPtr, nNewSize );
        }
        else
        {
            return alloc_T<T>( nNewSize );
        }
    }
    template <typename T> T * ReallocateCountGenericDefault( T * pPtr, const size_t nNewCount, void *const pUserData )
    {
        (void)pUserData;

        if ( pPtr )
        {
            return realloc_nT<T>( pPtr, nNewCount );
        }
        else
        {
            return alloc_nT<T>( nNewCount );
        }
    }
    template <typename T> void FreeGenericDefault( T *const pPtr, void *const pUserData ) { (void)pUserData; free( pPtr ); }
    template <typename T> constexpr AllocationFuncs<T> ALLOC_DEFAULT {
        .fnAllocate = AllocateGenericDefault<T>,
        .fnAllocateCount = AllocateCountGenericDefault<T>,
        .fnReallocate = ReallocateGenericDefault<T>,
        .fnReallocateCount = ReallocateCountGenericDefault<T>,
        .fnFree = FreeGenericDefault<T>
    };

    template <typename T> T * AllocateGenericUntracked( const size_t nSize, void *const pUserData ) { (void)pUserData; return static_cast<T *>( ::malloc( nSize ) ); }
    template <typename T> T * AllocateCountGenericUntracked( const size_t nCount, void *const pUserData ) { (void)pUserData; return static_cast<T *>( ::malloc( sizeof( T ) * nCount ) ); }
    template <typename T> T * ReallocateGenericUntracked( T * pPtr, const size_t nNewSize, void *const pUserData ) { (void)pUserData; return static_cast<T *>( ::realloc( pPtr, nNewSize ) ); }

    template <typename T> T * ReallocateCountGenericUntracked( T * pPtr, const size_t nNewCount, void *const pUserData ) { (void)pUserData; return static_cast<T *>( ::realloc( pPtr, sizeof( T ) * nNewCount ) ); }
    template <typename T> void FreeGenericUntracked( T *const pPtr, void *const pUserData ) { (void)pUserData; ::free( pPtr ); }
    template <typename T> constexpr AllocationFuncs<T> ALLOC_UNTRACKED {
        .fnAllocate = AllocateGenericUntracked<T>,
        .fnAllocateCount = AllocateCountGenericUntracked<T>,
        .fnReallocate = ReallocateGenericUntracked<T>,
        .fnReallocateCount = ReallocateCountGenericUntracked<T>,
        .fnFree = FreeGenericUntracked<T>
    };
};
