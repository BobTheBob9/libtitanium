#include "mem_external.hpp"
#include "mem_core.hpp"

#include <SDL.h>

/*void * operator new (size_t size) {
    return memory::externalMalloc( size );
}

void * operator new [](size_t size) {
    return memory::externalMalloc( size );
}

void operator delete(void * pData) throw() {
    memory::externalFree( pData );
}

void operator delete [](void * pData ) throw() {
    memory::externalFree( pData );
}*/

enum class EExternAllocationUser
{
    NONE,
    UNKNOWN,
    SDL,
    STB_IMAGE
};

namespace memory
{
    void SetExternMemoryFunctions_SDL()
    {
        SDL_SetMemoryFunctions( memory::externalMalloc, memory::externalCalloc, memory::externalRealloc, memory::externalFree );
    }
}
