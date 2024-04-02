#include "renderer.hpp"
#include "renderer_stringify.hpp"

#include <libtitanium/config/config.hpp>
#include <libtitanium/logger/logger.hpp>
#include <libtitanium/util/static_array.hpp>
#include <libtitanium/util/assert.hpp>

#include <webgpu/webgpu.h>

namespace renderer
{
    void LogGraphicsDeviceDescription( WGPUAdapter wgpuGraphicsDevice )
    {
        WGPUAdapterProperties wgpuGraphicsDeviceProperties;
        wgpuAdapterGetProperties( wgpuGraphicsDevice, &wgpuGraphicsDeviceProperties );

        logger::Info(
            "Found Adapter: %s %s, using driver %s on %s" ENDL,
            WGPUAdapterTypeToString( wgpuGraphicsDeviceProperties.adapterType ), wgpuGraphicsDeviceProperties.name,
            wgpuGraphicsDeviceProperties.driverDescription, WGPUBackendTypeToString( wgpuGraphicsDeviceProperties.backendType )
        );

        // TODO: adapter limits and features
    }

    bool Device::Create( Device *const o_renderDevice )
    {
        // create an instance of the webgpu library
        const WGPUInstanceDescriptor wgpuCreateInstanceDescriptor = {};
        WGPUInstance wgpuInstance = o_renderDevice->internal.wgpuInstance = wgpuCreateInstance( &wgpuCreateInstanceDescriptor );

        // get a handle to our physical gpu
        WGPUAdapter wgpuGraphicsDevice = o_renderDevice->internal.wgpuGraphicsDevice = [ wgpuInstance ]()
        {
            // TODO: probably make this all user-configurable
            // also TODO: don't get them from convars!!! need to figure out a proper interface for "most applications want these as convars, but some won't"
            const WGPURequestAdapterOptions wgpuRequestAdapterOptions = {
                .powerPreference = WGPUPowerPreference_HighPerformance,
                .backendType = WGPUBackendType_Undefined,
                .forceFallbackAdapter = false
            };

            // TODO: a function to print to a buf for WGPURequestAdapterOptions, which we can format into a log

            WGPUAdapter r_wgpuGraphicsDevice = nullptr;
            {
                auto fnRequestedGraphicsDevice = +[]( const WGPURequestAdapterStatus requestStatus, WGPUAdapter wgpuNewDevice, const char *const pszMessage, void *const o_wgpuDevice )
                {
                    if ( requestStatus == WGPURequestAdapterStatus_Success)
                    {
                        *reinterpret_cast<WGPUAdapter *>( o_wgpuDevice ) = wgpuNewDevice;
                    }
                    else
                    {
                        logger::Info( "wgpuInstanceRequestAdapter failed (%s): %s" ENDL, WGPURequestAdapterStatusToString( requestStatus ),  pszMessage );
                    }
                };

                wgpuInstanceRequestAdapter( wgpuInstance, &wgpuRequestAdapterOptions, fnRequestedGraphicsDevice, &r_wgpuGraphicsDevice );
            }

            return r_wgpuGraphicsDevice;
        }();

        assert::Release( wgpuGraphicsDevice, "No valid graphics device" );

        LogGraphicsDeviceDescription( wgpuGraphicsDevice );

        // construct a "virtual" graphics adapter
        // this lets us do stuff address a gpu that's more limited than our actual, physical one, great for dev!
        WGPUDevice wgpuVirtualDevice = o_renderDevice->internal.wgpuVirtualDevice = [ wgpuGraphicsDevice ]()
        {
            const WGPUDeviceDescriptor wgpuRequestDeviceOptions = {};

            WGPUDevice r_wgpuVirtualDevice = nullptr;
            {
                auto fnRequestedVirtualDevice = +[]( const WGPURequestDeviceStatus requestStatus, WGPUDevice wgpuNewVirtualDevice, const char *const pszMessage, void *const o_wgpuVirtualDevice )
                {
                    if ( requestStatus == WGPURequestDeviceStatus_Success )
                    {
                        *reinterpret_cast<WGPUDevice *>( o_wgpuVirtualDevice ) = wgpuNewVirtualDevice;
                    }
                    else
                    {
                        logger::Info( "wgpuAdapterRequestDevice failed (%s): %s" ENDL, WGPURequestDeviceStatusToString( requestStatus ), pszMessage );
                    }
                };

                wgpuAdapterRequestDevice( wgpuGraphicsDevice, &wgpuRequestDeviceOptions, fnRequestedVirtualDevice, &r_wgpuVirtualDevice );
            }

            return r_wgpuVirtualDevice;
        }();

        assert::Release( wgpuVirtualDevice, "No valid virtual graphics device" );

        o_renderDevice->internal.wgpuDeviceQueue = wgpuDeviceGetQueue( wgpuVirtualDevice );

        return true;
    }

    void Device::Destroy( Device *const pDevice )
    {
        wgpuDeviceDestroy( pDevice->internal.wgpuVirtualDevice );
        wgpuAdapterRelease( pDevice->internal.wgpuGraphicsDevice );
        wgpuInstanceRelease( pDevice->internal.wgpuInstance );
    }
}
