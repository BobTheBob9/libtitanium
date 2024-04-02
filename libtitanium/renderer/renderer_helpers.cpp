#include "renderer.hpp"
#include "renderer_helpers.hpp"

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/maths.hpp>
#include <libtitanium/util/static_array.hpp>

#include <libtitanium/logger/logger.hpp>

#include <webgpu/webgpu.h>

namespace renderer::helpers
{
    GPUFunctionArgs DEVTEMP_CreatePlane( Device *const pRenderDevice )
    {
        // plane vertices
        util::maths::Vec3<f32> plane[] = {
            { .x = 0.5f,  .y = -0.5f },
            { .x = 0.5f,  .y =  0.5f },
            { .x = -0.5f, .y =  0.5f },
            { .x = 0.5f,  .y = -0.5f },
            { .x = -0.5f, .y =  0.5f },
            { .x = -0.5f, .y = -0.5f },
        };

        GPUFunctionArgs r_args = {
            .meshType = GPUFunctionArgs::eMeshType::VERTEX,
            .internal = {
                .vertexBufSize = util::StaticArray_Length( plane ) * sizeof( util::maths::Vec3<f32> ), .vertexBufNumVertices = util::StaticArray_Length( plane )
            }
        };

        WGPUBufferDescriptor wgpuVertexBufferDescriptor = {
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
            .size = r_args.internal.vertexBufSize
        };
        r_args.internal.wgpuVertexBuffer = wgpuDeviceCreateBuffer( pRenderDevice->internal.wgpuVirtualDevice, &wgpuVertexBufferDescriptor );
        wgpuQueueWriteBuffer( pRenderDevice->internal.wgpuDeviceQueue, r_args.internal.wgpuVertexBuffer, /*bufferOffset*/ 0, plane, r_args.internal.vertexBufSize );

        logger::Info( "%s: %i bytes, %i verts" ENDL, __FUNCTION__, r_args.internal.vertexBufSize, r_args.internal.vertexBufNumVertices );

        return r_args;
    }

    void DEVTEMP_CreateUniformArgs( Device *const pRenderDevice, GPUFunction *const pgpuFunc, GPUFunctionArgs *const pArgs, size_t uniformSize, const void *const uniformValue )
    {
        WGPUBufferDescriptor wgpuUniformBufferDescriptor = {
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
            .size = uniformSize
        };
        pArgs->internal.wgpuUniformBuffer = wgpuDeviceCreateBuffer( pRenderDevice->internal.wgpuVirtualDevice, &wgpuUniformBufferDescriptor );

        WGPUBindGroupEntry wgpuUniformBinding = {
            .binding = 0,
            .buffer = pArgs->internal.wgpuUniformBuffer,
            .offset = 0,
            .size = uniformSize
        };
        WGPUBindGroupDescriptor wgpuUniformBindGroupDescriptor = {
            .layout = pgpuFunc->internal.wgpuFunctionUniformBindGroup,
            .entryCount = 1,
            .entries = &wgpuUniformBinding
        };
        pArgs->internal.wgpuUniformBindGroup = wgpuDeviceCreateBindGroup( pRenderDevice->internal.wgpuVirtualDevice, &wgpuUniformBindGroupDescriptor );

        /*
        WGPUSamplerDescriptor wgpuTextureSamplerDescriptor = {
            .magFilter = WGPUFilterMode_Linear,
            .minFilter = WGPUFilterMode_Linear,
            .mipmapFilter = WGPUMipmapFilterMode_Linear,
            .lodMaxClamp = 8.f,
            .maxAnisotropy = 1,
        };
        pArgs->internal.wgpuTextureSampler = wgpuDeviceCreateSampler( pRenderDevice->internal.wgpuVirtualDevice, &wgpuTextureSamplerDescriptor );

        WGPUBindGroupEntry wgpuSamplerBinding = {
            .binding = 0,
            .sampler = pArgs->internal.wgpuTextureSampler
        };
        WGPUBindGroupDescriptor wgpuSamplerBindGroupDescriptor = {
            .layout = pgpuFunc->internal.wgpuFunctionSamplerBindGroup,
            .entryCount = 1,
            .entries = &wgpuSamplerBinding
        };
        pArgs->internal.wgpuTextureSamplerBindGroup = wgpuDeviceCreateBindGroup( pRenderDevice->internal.wgpuVirtualDevice, &wgpuSamplerBindGroupDescriptor );
        */

        DEVTEMP_UpdateUniformArgs( pRenderDevice, pArgs, uniformSize, uniformValue );
    }

    void DEVTEMP_UpdateUniformArgs( Device *const pRenderDevice, GPUFunctionArgs *const pArgs, size_t uniformSize, const void *const uniformValue )
    {
        wgpuQueueWriteBuffer( pRenderDevice->internal.wgpuDeviceQueue, pArgs->internal.wgpuUniformBuffer, /*bufferOffset*/ 0, uniformValue, uniformSize );
    }
}
