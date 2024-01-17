#pragma once

// TODO: probably yoink most of this

#include <SDL.h>
#include <SDL_syswm.h>

#include <libtitanium/util/maths.hpp>

namespace sys::sdl
{
    util::maths::Vec2<u32> GetWindowSizeVector( SDL_Window *const psdlWindow );
}
