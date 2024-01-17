#include "renderer.hpp"

#include <libtitanium/logger/logger.hpp>
#include <libtitanium/config/config.hpp>
#include <libtitanium/renderer/renderer_stringify.hpp>
#include <webgpu/webgpu.h>

namespace renderer
{
    static bool s_bPreferLowPowerAdapter = false;
    static bool s_bForceFallbackAdapter = false;
    config::Var * pcvarPreferLowPowerAdapter = config::RegisterVar( "renderer::device::preferlowpoweradapter", config::EFVarUsageFlags::STARTUP, config::VARF_BOOL, &s_bPreferLowPowerAdapter );
    config::Var * pcvarForceFallbackAdapter = config::RegisterVar( "renderer::device::forcefallbackadapter", config::EFVarUsageFlags::STARTUP, config::VARF_BOOL, &s_bForceFallbackAdapter );

    void InitialisePhysicalRenderingDevice( TitaniumPhysicalRenderingDevice *const pRendererDevice )
    {
        logger::Info( "Initialising wgpu rendering device..." ENDL );
        //logger::Info( "wgpu version is %i" ENDL, wgpuGetVersion() ); // nonstandard :<

        const WGPUInstanceDescriptor wgpuCreateInstanceDescriptor {};
        WGPUInstance wgpuInstance = pRendererDevice->m_wgpuInstance = wgpuCreateInstance( &wgpuCreateInstanceDescriptor );

        // Request graphics adapter from instance
        WGPUAdapter wgpuGraphicsAdapter = pRendererDevice->m_wgpuGraphicsAdapter = [ wgpuInstance ]()
        {
            const WGPURequestAdapterOptions wgpuAdapterOptions {
                // note: .compatibleSurface could be used here, but we don't have a surface at this point, fortunately it isn't required
                .powerPreference = s_bPreferLowPowerAdapter ? WGPUPowerPreference_LowPower : WGPUPowerPreference_HighPerformance,
                .forceFallbackAdapter = s_bPreferLowPowerAdapter
            };

            const char * pszLogAdapter = "unknown";
            if ( wgpuAdapterOptions.forceFallbackAdapter )
            {
                pszLogAdapter = "fallback";
            }
            else
            {
                if ( wgpuAdapterOptions.powerPreference == WGPUPowerPreference_LowPower )
                {
                    pszLogAdapter = "low power";
                }
                else if ( wgpuAdapterOptions.powerPreference == WGPUPowerPreference_HighPerformance )
                {
                    pszLogAdapter = "high performance";
                }
            }

            logger::Info( "Requesting %s adapter..." ENDL, pszLogAdapter );

			WGPUAdapter r_wgpuGraphicsAdapter;
            wgpuInstanceRequestAdapter(
                wgpuInstance,
                &wgpuAdapterOptions,
                []( const WGPURequestAdapterStatus wgpuRequestAdapterStatus, WGPUAdapter wgpuAdapter, const char *const pszMessage, void *const pUserdata )
                {
                    logger::Info( "wgpuInstanceRequestAdapter returned %s, with message: %s" ENDL, WGPURequestAdapterStatusToString( wgpuRequestAdapterStatus ), pszMessage );

                    if ( wgpuRequestAdapterStatus == WGPURequestAdapterStatus_Success )
                    {
                       *reinterpret_cast<WGPUAdapter *>( pUserdata ) = wgpuAdapter;
                    }
               },
               &r_wgpuGraphicsAdapter
            );

            return r_wgpuGraphicsAdapter;
        }();

        // Print info about the graphics adapter
        [ wgpuGraphicsAdapter ]()
        {
            WGPUAdapterProperties wgpuAdapterProperties {};
            wgpuAdapterGetProperties( wgpuGraphicsAdapter, &wgpuAdapterProperties );

            logger::Info(
                "Found Adapter: %s %s, using driver %s on %s" ENDL,
                WGPUAdapterTypeToString( wgpuAdapterProperties.adapterType ),
                wgpuAdapterProperties.name,

                wgpuAdapterProperties.driverDescription,

                WGPUBackendTypeToString( wgpuAdapterProperties.backendType )
            );

            WGPUSupportedLimits wgpuAdapterLimits;
            wgpuAdapterGetLimits( wgpuGraphicsAdapter, &wgpuAdapterLimits );

            logger::Info( "Adapter Limits:" ENDL
                          "\tmaxTextureDimension1D = %u" ENDL
                          "\tmaxTextureDimension2D = %u" ENDL
                          "\tmaxTextureDimension3D = %u" ENDL
                          "\tmaxTextureArrayLayers = %u" ENDL
                          "\tmaxBindGroups = %u" ENDL
                          "\tmaxBindingsPerBindGroup = %u" ENDL
                          "\tmaxDynamicUniformBuffersPerPipelineLayout = %u" ENDL
                          "\tmaxDynamicStorageBuffersPerPipelineLayout = %u" ENDL
                          "\tmaxSampledTexturesPerShaderStage = %u" ENDL
                          "\tmaxSamplersPerShaderStage = %u" ENDL
                          "\tmaxStorageBuffersPerShaderStage = %u" ENDL
                          "\tmaxStorageTexturesPerShaderStage = %u" ENDL
                          "\tmaxUniformBuffersPerShaderStage = %u" ENDL
                          "\tmaxUniformBufferBindingSize = %llu" ENDL
                          "\tmaxStorageBufferBindingSize = %llu" ENDL
                          "\tminUniformBufferOffsetAlignment = %u" ENDL
                          "\tminStorageBufferOffsetAlignment = %u" ENDL
                          "\tmaxVertexBuffers = %u" ENDL
                          "\tmaxBufferSize = %llu" ENDL
                          "\tmaxVertexAttributes = %u" ENDL
                          "\tmaxVertexBufferArrayStride = %u" ENDL
                          "\tmaxInterStageShaderComponents = %u" ENDL
                          "\tmaxInterStageShaderVariables = %u" ENDL
                          "\tmaxColorAttachments = %u" ENDL
                          "\tmaxColorAttachmentBytesPerSample = %u" ENDL
                          "\tmaxComputeWorkgroupStorageSize = %u" ENDL
                          "\tmaxComputeInvocationsPerWorkgroup = %u" ENDL
                          "\tmaxComputeWorkgroupSizeX = %u" ENDL
                          "\tmaxComputeWorkgroupSizeY = %u" ENDL
                          "\tmaxComputeWorkgroupSizeZ = %u" ENDL
                          "\tmaxComputeWorkgroupsPerDimension = %u" ENDL,
                          wgpuAdapterLimits.limits.maxTextureDimension1D,
                          wgpuAdapterLimits.limits.maxTextureDimension2D,
                          wgpuAdapterLimits.limits.maxTextureDimension3D,
                          wgpuAdapterLimits.limits.maxTextureArrayLayers,
                          wgpuAdapterLimits.limits.maxBindGroups,
                          wgpuAdapterLimits.limits.maxBindingsPerBindGroup,
                          wgpuAdapterLimits.limits.maxDynamicUniformBuffersPerPipelineLayout,
                          wgpuAdapterLimits.limits.maxDynamicStorageBuffersPerPipelineLayout,
                          wgpuAdapterLimits.limits.maxSampledTexturesPerShaderStage,
                          wgpuAdapterLimits.limits.maxSamplersPerShaderStage,
                          wgpuAdapterLimits.limits.maxStorageBuffersPerShaderStage,
                          wgpuAdapterLimits.limits.maxStorageTexturesPerShaderStage,
                          wgpuAdapterLimits.limits.maxUniformBuffersPerShaderStage,
                          wgpuAdapterLimits.limits.maxUniformBufferBindingSize,
                          wgpuAdapterLimits.limits.maxStorageBufferBindingSize,
                          wgpuAdapterLimits.limits.minUniformBufferOffsetAlignment,
                          wgpuAdapterLimits.limits.minStorageBufferOffsetAlignment,
                          wgpuAdapterLimits.limits.maxVertexBuffers,
                          wgpuAdapterLimits.limits.maxBufferSize,
                          wgpuAdapterLimits.limits.maxVertexAttributes,
                          wgpuAdapterLimits.limits.maxVertexBufferArrayStride,
                          wgpuAdapterLimits.limits.maxInterStageShaderComponents,
                          wgpuAdapterLimits.limits.maxInterStageShaderVariables,
                          wgpuAdapterLimits.limits.maxColorAttachments,
                          wgpuAdapterLimits.limits.maxColorAttachmentBytesPerSample,
                          wgpuAdapterLimits.limits.maxComputeWorkgroupStorageSize,
                          wgpuAdapterLimits.limits.maxComputeInvocationsPerWorkgroup,
                          wgpuAdapterLimits.limits.maxComputeWorkgroupSizeX,
                          wgpuAdapterLimits.limits.maxComputeWorkgroupSizeY,
                          wgpuAdapterLimits.limits.maxComputeWorkgroupSizeZ,
                          wgpuAdapterLimits.limits.maxComputeWorkgroupsPerDimension );

            WGPUFeatureName wgpuAdapterFeatures[ 32 ];
            size_t nFeatures = wgpuAdapterEnumerateFeatures( wgpuGraphicsAdapter, wgpuAdapterFeatures );

            if ( nFeatures )
            {
                logger::Info( "Adapter has features: " ENDL );

                for ( uint i = 0; i < nFeatures; i++ )
                {
                    // TODO: track down why we're getting invalid enum values in this
                    logger::Info( "\t%s" ENDL, WGPUFeatureNameToString( wgpuAdapterFeatures[ i ] ) );
                }
            }
        }();
    }

	void ShutdownDevice( TitaniumPhysicalRenderingDevice *const pRendererDevice )
    {
        wgpuAdapterRelease( pRendererDevice->m_wgpuGraphicsAdapter );
        wgpuInstanceRelease( pRendererDevice->m_wgpuInstance );
    }
}
