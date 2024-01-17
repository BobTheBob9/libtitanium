#pragma once

#include "mem_core.hpp"

#ifndef HAS_CUSTOM_EXTERN_ALLOCS
    #define HAS_CUSTOM_EXTERN_ALLOCS 1
#endif // #ifndef HAS_TRACK_EXTERN_ALLOCS

#if HAS_CUSTOM_EXTERN_ALLOCS

    /*
    *  For libraries that allocate memory, it's helpful for us to be able to profile their allocations like we can for main code
    *  For this, we need to override allocation functions that the library uses
    *
    *  NOTE: if we really need to override malloc in a library we can't control, i think there's no reason we couldn't just:
    *      #define malloc memory::externalMalloc, assuming we're building said library from source
    *
    *  but that's definitely not ideal
    */

    // custom stbi memory functions
    #define STBI_MALLOC( nBytes ) memory::externalMalloc( nBytes )
    #define STBI_REALLOC( pMemoryToRealloc, nBytes ) memory::externalRealloc( pMemoryToRealloc, nBytes )
    #define STBI_FREE( pMemoryToFree ) memory::externalFree( pMemoryToFree )

    namespace memory
    {
        // TODO: should probably ifdef this stuff out when not using sdl? but at the moment we always use sdl
        void SetExternMemoryFunctions_SDL();
    }

#endif // #if HAS_CUSTOM_EXTERN_ALLOCS
