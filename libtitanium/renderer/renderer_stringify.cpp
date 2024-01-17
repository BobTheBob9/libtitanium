#include "renderer_stringify.hpp"

namespace renderer
{
    const char * WGPURequestAdapterStatusToString( const WGPURequestAdapterStatus ewgpuRequestAdapterStatus )
    {
        switch ( ewgpuRequestAdapterStatus )
        {
            case WGPURequestAdapterStatus_Success:
            {
                return "Success";
            }

            case WGPURequestAdapterStatus_Unavailable:
            {
                return "Unavailable";
            }

            case WGPURequestAdapterStatus_Error:
            {
                return "Error";
            }

            case WGPURequestAdapterStatus_Unknown:
            case WGPURequestAdapterStatus_Force32:
            {
                return "Unknown";
            }
        }
    }

    const char * WGPURequestDeviceStatusToString( const WGPURequestDeviceStatus ewgpuRequestDeviceStatus )
    {
        switch ( ewgpuRequestDeviceStatus )
        {
            case WGPURequestDeviceStatus_Success:
            {
                return "Success";
            }

            case WGPURequestDeviceStatus_Error:
            {
                return "Error";
            }

            case WGPURequestDeviceStatus_Unknown:
            case WGPURequestDeviceStatus_Force32:
            {
                return "Unknown";
            }
        }
    }

    const char * WGPUAdapterTypeToString( const WGPUAdapterType ewgpuAdapterType )
    {
        switch ( ewgpuAdapterType )
        {
            case WGPUAdapterType_DiscreteGPU:
            {
                return "Discrete GPU";
            }

            case WGPUAdapterType_IntegratedGPU:
            {
                return "Integrated GPU";
            }

            case WGPUAdapterType_CPU:
            {
                return "CPU";
            }

            case WGPUAdapterType_Unknown:
            case WGPUAdapterType_Force32:
            {
                return "Unknown";
            }
        }
    }

    const char * WGPUBackendTypeToString( const WGPUBackendType ewgpuBackendType )
    {
        switch ( ewgpuBackendType )
        {
            case WGPUBackendType_WebGPU:
            {
                return "WebGPU";
            }

            case WGPUBackendType_D3D11:
            {
                return "D3D11";
            }

            case WGPUBackendType_D3D12:
            {
                return "D3D11";
            }

            case WGPUBackendType_Metal:
            {
                return "Metal";
            }

            case WGPUBackendType_Vulkan:
            {
                return "Vulkan";
            }

            case WGPUBackendType_OpenGL:
            {
                return "OpenGL";
            }

            case WGPUBackendType_OpenGLES:
            {
                return "OpenGLES";
            }

            case WGPUBackendType_Null:
            case WGPUBackendType_Force32:
            case WGPUBackendType_Undefined:
            {
                return "Unknown";
            }
        }
    }

    const char * WGPUFeatureNameToString( const WGPUFeatureName ewgpuFeatureName )
    {
        switch ( ewgpuFeatureName )
        {
            case WGPUFeatureName_DepthClipControl:
            {
                return "DepthClipControl";
            }

            case WGPUFeatureName_Depth32FloatStencil8:
            {
                return "Depth32FloatStencil8";
            }

            case WGPUFeatureName_TimestampQuery:
            {
                return "TimestampQuery";
            }

            //case WGPUFeatureName_PipelineStatisticsQuery:
            //{
            //    return "PipelineStatisticsQuery";
            //}

            case WGPUFeatureName_TextureCompressionBC:
            {
                return "TextureCompressionBC";
            }

            case WGPUFeatureName_TextureCompressionETC2:
            {
                return "TextureCompressionETC2";
            }

            case WGPUFeatureName_TextureCompressionASTC:
            {
                return "TextureCompressionASTC";
            }

            case WGPUFeatureName_IndirectFirstInstance:
            {
                return "IndirectFirstInstance";
            }

            case WGPUFeatureName_ShaderF16:
            {
                return "ShaderF16";
            }

            case WGPUFeatureName_RG11B10UfloatRenderable:
            {
                return "RG11B10UfloatRenderable";
            }

            case WGPUFeatureName_BGRA8UnormStorage:
            {
                return "BGRA8UnormStorage";
            }

            case WGPUFeatureName_Undefined:
            case WGPUFeatureName_Force32:
            case WGPUFeatureName_Float32Filterable:
            default: // not a fan of this, but ewgpuFeatureName is sometimes not a correct value, and so we get UB with this
            {
                return "Unknown";
            }
        }
    }

    const char * WGPUErrorTypeToString( const WGPUErrorType ewgpuErrorType )
    {
        switch ( ewgpuErrorType )
        {
            case WGPUErrorType_Validation:
            {
                return "Validation";
            }

            case WGPUErrorType_OutOfMemory:
            {
                return "Out of Memory";
            }

            case WGPUErrorType_Internal:
            {
                return "Internal";
            }

            case WGPUErrorType_DeviceLost:
            {
                return "Device Lost";
            }

            case WGPUErrorType_NoError:
            {
                return "No Error";
            }

            case WGPUErrorType_Unknown:
            case WGPUErrorType_Force32:
            {
                return "Unknown";
            }
        }
    }

    const char * WGPUPresentModeToString( const WGPUPresentMode ewgpuPresentMode )
    {
        switch ( ewgpuPresentMode )
        {
            case WGPUPresentMode_Immediate:
            {
                return "Immediate";
            }

            case WGPUPresentMode_Mailbox:
            {
                return "VSync - Mailbox";
            }

            case WGPUPresentMode_Fifo:
            {
                return "VSync - FIFO";
            }

            case WGPUPresentMode_FifoRelaxed: // temp
            case WGPUPresentMode_Force32:
            {
                return "Unknown";
            }
        }
    }
}
