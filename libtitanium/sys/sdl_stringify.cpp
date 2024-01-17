#include "sdl_stringify.hpp"

namespace sys::sdl
{
    const util::data::StringBuf<32> VersionToString( const SDL_version sdlVersion )
    {
        return util::data::StringBuf<32>( "%i.%i.%i", sdlVersion.major, sdlVersion.minor, sdlVersion.patch );
    }

    const util::data::StringBuf<32> StringRuntimeVersion()
    {
        SDL_version sdlRuntimeVersion;
        SDL_GetVersion( &sdlRuntimeVersion );

        return VersionToString( sdlRuntimeVersion );
    }

    const util::data::StringBuf<32> StringCompilationVersion()
    {
        SDL_version sdlCompiledVersion;
        SDL_VERSION( &sdlCompiledVersion );

        return VersionToString( sdlCompiledVersion );
    }

    const char * SysWMToString( const SDL_SYSWM_TYPE esdlWMType )
    {
        switch ( esdlWMType )
        {
            case SDL_SYSWM_UNKNOWN:
            {
                return "SDL_SYSWM_UNKNOWN";
            }

            case SDL_SYSWM_X11:
            {
                return "Linux: X11";
            }

            case SDL_SYSWM_WAYLAND:
            {
                return "Linux: Wayland";
            }

            case SDL_SYSWM_WINDOWS:
            {
                return "Windows";
            }

            case SDL_SYSWM_COCOA:
            {
                return "MacOS: Cocoa";
            }

            case SDL_SYSWM_DIRECTFB:
            case SDL_SYSWM_UIKIT:
            case SDL_SYSWM_MIR:
            case SDL_SYSWM_WINRT:
            case SDL_SYSWM_ANDROID:
            case SDL_SYSWM_VIVANTE:
            case SDL_SYSWM_OS2:
            case SDL_SYSWM_HAIKU:
            case SDL_SYSWM_KMSDRM:
            case SDL_SYSWM_RISCOS:
            {
                return "Unsupported";
            }
        }
    }
}
