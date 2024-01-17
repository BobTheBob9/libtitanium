#include "renderer.hpp"

#include <libtitanium/util/maths.hpp>

#include <webgpu/webgpu.h>
#include <imgui.h>
#include <imgui_impl_wgpu.h>

#if TITANIUM_SDL
    #include <SDL_syswm.h>
#endif // #if TITANIUM_SDL

namespace renderer
{
    constexpr WGPUTextureFormat DEPTH_TEXTURE_FORMAT = WGPUTextureFormat_Depth24Plus;
    void CreateDrawTargetDepthTextureForResolution( DrawTarget *const pDrawTarget, Device *const pRenderDevice, const util::maths::Vec2<uint> resolution )
    {
        const WGPUTextureDescriptor wgpuCreateDepthTextureDescriptor = {
            .usage = WGPUTextureUsage_RenderAttachment,
            .dimension = WGPUTextureDimension_2D,
            .size = { resolution.x, resolution.y, 1 },
            .format = DEPTH_TEXTURE_FORMAT,
            .mipLevelCount = 1,
            .sampleCount = 1,
            .viewFormatCount = 1,
            .viewFormats = &DEPTH_TEXTURE_FORMAT
        };
        pDrawTarget->internal.wgpuDepthTexture = wgpuDeviceCreateTexture( pRenderDevice->internal.wgpuVirtualDevice, &wgpuCreateDepthTextureDescriptor );

        const WGPUTextureViewDescriptor wgpuCreateDepthTextureViewDescriptor = {
            .format = DEPTH_TEXTURE_FORMAT,
            .dimension = WGPUTextureViewDimension_2D,
            .mipLevelCount = 1,
            .arrayLayerCount = 1,
            .aspect = WGPUTextureAspect_DepthOnly
        };
        pDrawTarget->internal.wgpuDepthTextureView = wgpuTextureCreateView( pDrawTarget->internal.wgpuDepthTexture, &wgpuCreateDepthTextureViewDescriptor );
    }

    void DestroyDrawTargetDepthTexture( DrawTarget *const pDrawTarget )
    {
        wgpuTextureDestroy( pDrawTarget->internal.wgpuDepthTexture );
        wgpuTextureRelease( pDrawTarget->internal.wgpuDepthTexture );
        wgpuTextureViewRelease( pDrawTarget->internal.wgpuDepthTextureView );
    }

    // platform-specific code... here be dragons...
#if TITANIUM_SDL
    void ConfigureWindowWGPUSurface( WGPUSurface wgpuWindowSurface, Device *const pRenderDevice, const util::maths::Vec2<uint> resolution )
    {
        WGPUSurfaceCapabilities wgpuWindowSurfaceCaps; wgpuSurfaceGetCapabilities( wgpuWindowSurface, pRenderDevice->internal.wgpuGraphicsDevice, &wgpuWindowSurfaceCaps );

        // determine the right optimal present mode n stuff
        const WGPUPresentMode wgpuIdealPresentMode = WGPUPresentMode_Immediate; // TODO: make configurable by caller
        WGPUPresentMode wgpuBestPresentMode = WGPUPresentMode_Mailbox; // mailbox (vsync) should be supported like, everywhere
        for ( uint i = 0; i < wgpuWindowSurfaceCaps.presentModeCount; i++ )
        {
            if ( wgpuWindowSurfaceCaps.presentModes[ i ] == wgpuIdealPresentMode )
            {
                wgpuBestPresentMode = wgpuIdealPresentMode;
                break;
            }
        }

        const WGPUTextureFormat wgpuSurfaceFormat = wgpuSurfaceGetPreferredFormat( wgpuWindowSurface, pRenderDevice->internal.wgpuGraphicsDevice );
        const WGPUSurfaceConfiguration wgpuConfigureSurfaceParams = {
            .device = pRenderDevice->internal.wgpuVirtualDevice,

            .format = wgpuSurfaceFormat,
            .usage = WGPUTextureUsage_RenderAttachment,

            .width = resolution.x,
            .height = resolution.y,

            .presentMode = wgpuBestPresentMode
        };
        wgpuSurfaceConfigure( wgpuWindowSurface, &wgpuConfigureSurfaceParams );
    }

    DrawTarget DrawTarget::CreateFromSysWindow_SDL( Device *const pRenderDevice, SDL_Window *const psdlWindow )
    {
        DrawTarget r_drawTarget;

        SDL_SysWMinfo sdlPlatformWindowInfo;
        SDL_VERSION( &sdlPlatformWindowInfo.version );
        SDL_GetWindowWMInfo( psdlWindow , &sdlPlatformWindowInfo );

        // SDL2 has no native WGPU support, so we need to perform the surface creation process manually
        WGPUSurfaceDescriptor wgpuCreateSurfaceDescriptor;

    #ifdef linux
        const WGPUSurfaceDescriptorFromXlibWindow wgpuSurfaceDescX11 = {
            .chain = { .sType = WGPUSType_SurfaceDescriptorFromXlibWindow },

            .display = sdlPlatformWindowInfo.info.x11.display,
            .window = static_cast<u32>( sdlPlatformWindowInfo.info.x11.window )
        };

        const WGPUSurfaceDescriptorFromWaylandSurface wgpuSurfaceDescWayland = {
            .chain = { .sType = WGPUSType_SurfaceDescriptorFromWaylandSurface },

            .display = sdlPlatformWindowInfo.info.wl.display,
            .surface = sdlPlatformWindowInfo.info.wl.surface
        };

        if ( sdlPlatformWindowInfo.subsystem == SDL_SYSWM_X11 )
        {
            wgpuCreateSurfaceDescriptor.nextInChain = reinterpret_cast<const WGPUChainedStruct *>( &wgpuSurfaceDescX11 );
        }
        else if ( sdlPlatformWindowInfo.subsystem == SDL_SYSWM_WAYLAND )
        {
            wgpuCreateSurfaceDescriptor.nextInChain = reinterpret_cast<const WGPUChainedStruct *>( &wgpuSurfaceDescWayland );
        }
    #else // #ifdef linux
        #error "Renderer - DrawTarget::CreateFromSysWindow_SDL doesn't support the platform it's being compiled for :c"
    #endif // #else // #ifdef linux

        WGPUSurface wgpuWindowSurface = wgpuInstanceCreateSurface( pRenderDevice->internal.wgpuInstance, &wgpuCreateSurfaceDescriptor );
        // LogWGPUWindowSurface( wgpuWindowSurface, Device *const pRenderDevice )

        int windowWidth, windowHeight;
        SDL_GetWindowSize( psdlWindow, &windowWidth, &windowHeight );
        ConfigureWindowWGPUSurface( wgpuWindowSurface, pRenderDevice, { .x = static_cast<uint>( windowWidth ), .y = static_cast<uint>( windowHeight ) } );
        CreateDrawTargetDepthTextureForResolution( &r_drawTarget, pRenderDevice, { .x = static_cast<uint>( windowWidth ), .y = static_cast<uint>( windowHeight ) } );

        // TODO: TEMP!!! this shouldn't really be in renderer code
        ImGui_ImplWGPU_Init( pRenderDevice->internal.wgpuVirtualDevice, 1, wgpuSurfaceGetPreferredFormat( wgpuWindowSurface, pRenderDevice->internal.wgpuGraphicsDevice ), DEPTH_TEXTURE_FORMAT );

        r_drawTarget.internal.wgpuTargetSurface = wgpuWindowSurface;

        return r_drawTarget;
    }

    void DrawTarget::RecreateFromSysWindow_SDL( DrawTarget *const pDrawTarget, Device *const pRenderDevice, SDL_Window *const psdlWindow )
    {
        DestroyDrawTargetDepthTexture( pDrawTarget );

        int windowWidth, windowHeight;
        SDL_GetWindowSize( psdlWindow, &windowWidth, &windowHeight );
        ConfigureWindowWGPUSurface( pDrawTarget->internal.wgpuTargetSurface, pRenderDevice, { .x = static_cast<uint>( windowWidth ), .y = static_cast<uint>( windowHeight ) } );
        CreateDrawTargetDepthTextureForResolution( pDrawTarget, pRenderDevice, { .x = static_cast<uint>( windowWidth ), .y = static_cast<uint>( windowHeight ) } );
    }
#endif // #if TITANIUM_SDL

    void DrawTarget::Destroy( DrawTarget *const pDrawTarget )
    {
        wgpuTextureViewRelease( pDrawTarget->internal.wgpuDepthTextureView );
        wgpuTextureRelease( pDrawTarget->internal.wgpuDepthTexture );
        wgpuTextureDestroy( pDrawTarget->internal.wgpuDepthTexture );
        wgpuSurfaceRelease( pDrawTarget->internal.wgpuTargetSurface );
    }
}
