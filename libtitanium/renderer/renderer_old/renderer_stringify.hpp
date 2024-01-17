#pragma once

#include <webgpu/webgpu.h>

#include <libtitanium/util/data/stringbuf.hpp>

namespace renderer
{
    const char * WGPURequestAdapterStatusToString( const WGPURequestAdapterStatus ewgpuRequestAdapterStatus );
    const char * WGPURequestDeviceStatusToString( const WGPURequestDeviceStatus ewgpuRequestDeviceStatus );
    const char * WGPUAdapterTypeToString( const WGPUAdapterType ewgpuAdapterType );
    const char * WGPUBackendTypeToString( const WGPUBackendType ewgpuBackendType );
    const char * WGPUFeatureNameToString( const WGPUFeatureName ewgpuFeatureName );
    const char * WGPUErrorTypeToString( const WGPUErrorType ewgpuErrorType );
    const char * WGPUPresentModeToString( const WGPUPresentMode ewgpuPresentMode );
}
