#pragma once

#define RESOURCE_HAS_CACHING 0 // temp uwu

// Whether we should cache files we already have loaded
// Changes a bunch of stuff, like making resource load/free do refcounting etc
#ifndef RESOURCE_HAS_CACHING
    #define RESOURCE_HAS_CACHING 1
#endif // #ifndef RESOURCE_HAS_CACHING

#include <libtitanium/util/data/span.hpp>
#include <libtitanium/memory/mem_core.hpp>
#include <libtitanium/resource/readbuf.hpp>

namespace resource
{
    struct ResourceLoader
    {
#if RESOURCE_HAS_CACHING
        // hashmap for caching, etc
#endif // #if RESOURCE_HAS_CACHING
    };

    void Initialise( ResourceLoader *const pResourceLoader );

    struct GetReadBufResult
    {
        bool success;
        ReadBuf readBuf;
    };

    GetReadBufResult GetReadBufForPath( ResourceLoader *const pResourceLoader, const util::data::Span<char> strPath );

    enum eLoadFailureReason : byte
    {
        // errors
        RESOURCE_NOT_FOUND,
        RESOURCE_FAILED_PARSE,
        RESOURCE_FAILED_CREATE_NATIVE_HANDLE
    };

    template <typename T> struct Resource
    {
        union
        {
            u64 resourceId;
            eLoadFailureReason failureReason;
        };

        T * data;
    };

    template <typename T, bool ( *FNParseFile )( ReadBuf rawDataReadBuf, T *const o_readData ) = T::ParseResource> Resource<T> LoadFromPath_T( ResourceLoader *const pResourceLoader, util::data::Span<char> strPath )
    {
#if RESOURCE_HAS_CACHING
        GetCachedParsedFile getCachedFile = GetCachedParsedFileForPath( strPath );
        if ( getCachedFile.success )
        {
            return { .failureReason = eLoadFailureReason::SUCCESS, .data = getCachedFile.data };
        }
#endif // #if RESOURCE_HAS_CACHING

        GetReadBufResult getReadBuf = GetReadBufForPath( nullptr, strPath );
        if ( !getReadBuf.success )
        {
            return { .failureReason = eLoadFailureReason::RESOURCE_NOT_FOUND, .data = nullptr };
        }

        // TODO: let parsers do the allocations here probably, but need an interface to let the resource system know about them for caching and cleanup purposes
        T * r_buf = memory::alloc_nT<T>( 1 );
        bool parseResult = FNParseFile( getReadBuf.readBuf, r_buf );
        if ( !parseResult )
        {
            return { .failureReason = eLoadFailureReason::RESOURCE_FAILED_PARSE, .data = nullptr };
        }

#if RESOURCE_HAS_CACHING
        CachedFileRefcountIncrement( strPath, nullptr /*TODO*/ );
#endif // #if RESOURCE_HAS_CACHING

        // TODO: gen actual id or pointer
        return { .resourceId = 1, .data = r_buf };
    }

    template <typename T, void ( *FNFreeFile )( T *const pFile ) = T::Cleanup> void FreeResource_T( Resource<T> resource )
    {
#if RESOURCE_HAS_CACHING
        // TODO: handle refcounting, only free if fully dereferenced
        return; // idk
#endif // #if RESOURCE_HAS_CACHING
        FNFreeFile( resource.data );
        memory::free( resource.data );
    }

    //void AttemptLoadNativeDeviceHandleFromPath_T( const util::data::Span<char> strPath, eLoadResult *const pLoadResult );

    /*int main()
    {
        resource::LoadResult mapLoad = resource::LoadFromPath_T<KineticMap>( SPAN( "maps/menu/lobby.kmap" ) );
        assert::Release( mapLoad.status == resource::LoadResult::eStatus::SUCCESS );
    }*/
}
