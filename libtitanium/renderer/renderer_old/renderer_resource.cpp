#include "imgui.h"
#include "renderer.hpp"
#include <webgpu/webgpu.h>

namespace renderer
{
    GPUModelHandle UploadModel( TitaniumRendererState *const pRendererState, const ::util::data::Span<ModelVertexAttributes> sVertices, const ::util::data::Span<u16> snIndexes )
    {
        const size_t nVertexBufSize = sVertices.nLength * sizeof( ModelVertexAttributes );
        WGPUBufferDescriptor wgpuVertexBufferDescriptor {
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
            .size = nVertexBufSize,
        };
        WGPUBuffer wgpuVertexBuffer = wgpuDeviceCreateBuffer( pRendererState->m_wgpuVirtualDevice, &wgpuVertexBufferDescriptor );
        wgpuQueueWriteBuffer( pRendererState->m_wgpuQueue, wgpuVertexBuffer, 0, sVertices.pData, nVertexBufSize );

        const size_t nIndexBufSize = snIndexes.nLength * sizeof( u16 );
        WGPUBufferDescriptor wgpuIndexBufferDescriptor {
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index,
            .size = nIndexBufSize
        };
        WGPUBuffer wgpuIndexBuffer = wgpuDeviceCreateBuffer( pRendererState->m_wgpuVirtualDevice, &wgpuIndexBufferDescriptor );
        wgpuQueueWriteBuffer( pRendererState->m_wgpuQueue, wgpuIndexBuffer, 0, snIndexes.pData, nIndexBufSize );

        return {
            .m_wgpuVertexBuffer = wgpuVertexBuffer,
            .m_nVertexBufferSize = nVertexBufSize,

            .m_wgpuIndexBuffer = wgpuIndexBuffer,
            .m_nIndexBufferSize = nIndexBufSize,
            .m_nIndexBufferCount = static_cast<int>( snIndexes.nLength )
        };
    }

    void FreeGPUModel( GPUModelHandle gpuModel )
    {
        wgpuBufferRelease( gpuModel.m_wgpuVertexBuffer );
        wgpuBufferRelease( gpuModel.m_wgpuIndexBuffer );
        wgpuBufferDestroy( gpuModel.m_wgpuVertexBuffer );
        wgpuBufferDestroy( gpuModel.m_wgpuIndexBuffer );
    }

	GPUTextureHandle UploadTexture( TitaniumRendererState *const pRendererState, const util::maths::Vec2<u16> vTextureSize, const WGPUTextureFormat wgpuTextureFormat, const byte *const pTextureData )
    {
        WGPUTextureDescriptor wgpuTextureDesc {
            .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
            .dimension = WGPUTextureDimension_2D,
            .size = { .width = vTextureSize.x, .height = vTextureSize.y, .depthOrArrayLayers = 1 },
            .format = wgpuTextureFormat,
            .mipLevelCount = 1,
            .sampleCount = 1,
        };
        WGPUTexture r_wgpuTexture = wgpuDeviceCreateTexture( pRendererState->m_wgpuVirtualDevice, &wgpuTextureDesc );

        WGPUTextureViewDescriptor wgpuTextureViewDesc {
            .format = wgpuTextureDesc.format,
            .dimension = WGPUTextureViewDimension_2D,
            .baseMipLevel = 0,
            .mipLevelCount = 1,
            .baseArrayLayer = 0,
            .arrayLayerCount = 1,
        };
        WGPUTextureView r_wgpuTextureView = wgpuTextureCreateView( r_wgpuTexture, &wgpuTextureViewDesc );

        WGPUImageCopyTexture wgpuTextureWriteDestDesc { .texture = r_wgpuTexture, .mipLevel = 0 };
        // TODO: this should handle determining bytesPerRow from texture format, ideally
        WGPUTextureDataLayout wgpuTextureSourceDesc { .bytesPerRow = 4 * wgpuTextureDesc.size.width, .rowsPerImage = wgpuTextureDesc.size.height };
        wgpuQueueWriteTexture( pRendererState->m_wgpuQueue, &wgpuTextureWriteDestDesc, pTextureData, wgpuTextureSourceDesc.bytesPerRow * wgpuTextureSourceDesc.rowsPerImage, &wgpuTextureSourceDesc, &wgpuTextureDesc.size );

        return { r_wgpuTexture, r_wgpuTextureView };
    }

    ImTextureID GPUTexture_GetImguiTexture( const GPUTextureHandle gpuTexture )
    {
        return static_cast<ImTextureID>( gpuTexture.m_wgpuTextureView );
    }

	void FreeGPUTexture( GPUTextureHandle gpuTexture )
    {
        wgpuTextureViewRelease( gpuTexture.m_wgpuTextureView );
        wgpuTextureRelease( gpuTexture.m_wgpuTexture );
    }
}
