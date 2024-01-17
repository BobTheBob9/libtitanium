#include "platform_sdl.hpp"

#include <libtitanium/logger/logger.hpp>
#include <libtitanium/sys/sdl_stringify.hpp>

namespace sys::sdl
{
    util::maths::Vec2<u32> GetWindowSizeVector( SDL_Window *const psdlWindow )
    {
        int nWindowWidth, nWindowHeight;
        SDL_GetWindowSize( psdlWindow, &nWindowWidth, &nWindowHeight );
        return util::maths::Vec2<u32> { .x = static_cast<u32>( nWindowWidth ), .y = static_cast<u32>( nWindowHeight ) };
    }
}
