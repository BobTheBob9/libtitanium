#pragma once

#include <SDL.h>
#include <SDL_syswm.h>

#include <libtitanium/util/data/stringbuf.hpp>

namespace sys::sdl
{
    const util::data::StringBuf<32> VersionToString( const SDL_version sdlVersion );
    const util::data::StringBuf<32> StringRuntimeVersion();
    const util::data::StringBuf<32> StringCompilationVersion();
    const char * SysWMToString( const SDL_SYSWM_TYPE esdlWMType );
}
