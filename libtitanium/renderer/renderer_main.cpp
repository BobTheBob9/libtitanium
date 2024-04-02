#include "renderer.hpp"

#include <libtitanium/util/assert.hpp>
#include <libtitanium/util/data/multitree.hpp>
#include <libtitanium/util/data/span.hpp>

#include <webgpu/webgpu.h>

namespace renderer
{
	// TODO: draw commands should essentially represent one step of a render
	// so like, a draw command might do something like "render this big list of objects with this shader!" we should provide helpers that let you easily dispatch common commands
	// ( e.g. draw list of 3d textured objects ) but other systems should be able to define their own ( like the gui system, for instance! )

	// also TODO: note, we probably want to be able to do nested draw commands for render textures n stuff
	// which does complicate things somewhat but oh well xd
	// maybe we can use the whole multitree struct again, i dunno idk fuck

	// oh fucking also TODO: if this ends up causing unnecessary allocs on render to build the command list i will shit myself
	// also: should this handle compute? or should that be its own thing? compute needs different args from a traditional draw pass

	void RunDrawCommands( Device *const pDevice, DrawTarget *const pDrawTarget, const util::data::Span<DrawCommand> drawCommands )
	{
		WGPUSurfaceTexture wgpuDrawSurfaceTexture; wgpuSurfaceGetCurrentTexture( pDrawTarget->internal.wgpuTargetSurface, &wgpuDrawSurfaceTexture );
		assert::Debug( wgpuDrawSurfaceTexture.status == WGPUSurfaceGetCurrentTextureStatus_Success, "Bad value in wgpuDrawSurfaceTexture.status" );

		WGPUTextureView wgpuDrawTexture = wgpuTextureCreateView( wgpuDrawSurfaceTexture.texture, nullptr );

		WGPUCommandEncoderDescriptor wgpuCreateCommandEncoderDescriptor = {};
		WGPUCommandEncoder wgpuCommandEncoder = wgpuDeviceCreateCommandEncoder( pDevice->internal.wgpuVirtualDevice, &wgpuCreateCommandEncoderDescriptor );
		{
			WGPURenderPassDepthStencilAttachment wgpuRenderPassDepthStencilDescriptor = {
				.view = pDrawTarget->internal.wgpuDepthTextureView,
				.depthLoadOp = WGPULoadOp_Clear,
				.depthStoreOp =  WGPUStoreOp_Store,
				.depthClearValue = 1.f,
				.depthReadOnly = false
			};

			WGPURenderPassColorAttachment wgpuRenderPassClearColourDescriptor = {
				.view = wgpuDrawTexture,
				.loadOp = WGPULoadOp_Clear,
				.storeOp = WGPUStoreOp_Store,
				.clearValue = { 0.05f, 0.05f, 0.05f, 1.f } // TODO: configurable colours
			};

			WGPURenderPassDescriptor wgpuRenderPassDescriptor = {
				.colorAttachmentCount = 1ull,
				.colorAttachments = &wgpuRenderPassClearColourDescriptor,
				.depthStencilAttachment  = &wgpuRenderPassDepthStencilDescriptor
			};

			WGPURenderPassEncoder wgpuRenderPass = wgpuCommandEncoderBeginRenderPass( wgpuCommandEncoder, &wgpuRenderPassDescriptor );
			{
				for ( uint i = 0; i < drawCommands.nLength; i++ )
				{
					DrawCommand *const pDrawCommand = &drawCommands.pData[ i ];

					// set the current function
					wgpuRenderPassEncoderSetPipeline( wgpuRenderPass, pDrawCommand->gpuFunction->internal.wgpuRenderPipeline );

					// then call it once with each set of provided args
					// TODO: possibly we could look into using render bundles to batch lots of similar calls here?
					for ( uint j = 0; j < drawCommands.pData[ i ].callArgs.nLength; j++ )
					{
						GPUFunctionArgs *const pCallArgs = &pDrawCommand->callArgs.pData[ j ];

						// set the args, uniforms, texture sampler and textures
						wgpuRenderPassEncoderSetBindGroup( wgpuRenderPass, /*groupIndex*/ 0, pCallArgs->internal.wgpuUniformBindGroup, /*dynamicOffsetCount*/ 0, /*dynamicOffsets*/nullptr );
						//wgpuRenderPassEncoderSetBindGroup( wgpuRenderPass, /*groupIndex*/ 1, pCallArgs->internal.wgpuTextureSamplerBindGroup, /*dynamicOffsetCount*/ 0, /*dynamicOffsets*/ nullptr );

						wgpuRenderPassEncoderSetVertexBuffer( wgpuRenderPass, /*slot*/ 0, pCallArgs->internal.wgpuVertexBuffer, /*offset*/ 0, pCallArgs->internal.vertexBufSize );
						if ( pCallArgs->meshType == GPUFunctionArgs::eMeshType::VERTEX )
						{
							wgpuRenderPassEncoderDraw( wgpuRenderPass, pCallArgs->internal.vertexBufNumVertices, /*instanceCount*/ 1, /*firstVertex*/ 0, /*firstInstance*/ 0 );
						}
						else if ( pCallArgs->meshType == GPUFunctionArgs::eMeshType::VERTEX_INDEX )
						{
							wgpuRenderPassEncoderSetIndexBuffer( wgpuRenderPass, pCallArgs->internal.wgpuVertexBuffer, WGPUIndexFormat_Uint16, pCallArgs->internal.indexBufOffset, pCallArgs->internal.indexBufSize );
							wgpuRenderPassEncoderDrawIndexed( wgpuRenderPass, pCallArgs->internal.indexBufNumIndices, /*instanceCount*/ 1, /*firstIndex*/ 0, /*baseVertex*/ 0, /*firstInstance*/ 0 );
						}

					}
				}
			}

			wgpuRenderPassEncoderEnd( wgpuRenderPass );
			wgpuRenderPassEncoderRelease( wgpuRenderPass );
		}

		WGPUCommandBufferDescriptor wgpuCreateCommandBufferDescriptor = {};
		WGPUCommandBuffer wgpuRenderCommand = wgpuCommandEncoderFinish( wgpuCommandEncoder, &wgpuCreateCommandBufferDescriptor );
		wgpuCommandEncoderRelease( wgpuCommandEncoder );

		wgpuTextureViewRelease( wgpuDrawTexture );

		wgpuQueueSubmit( pDevice->internal.wgpuDeviceQueue, 1, &wgpuRenderCommand );
		wgpuSurfacePresent( pDrawTarget->internal.wgpuTargetSurface );
	}


	/*void test()
	{
		DrawCommand draw3D = renderer::drawhelpers::Draw3DObjects( view, renderObjects );
		renderer::RunDrawCommands( &device, &target, { .nLength = 1, .pData = &draw3D });
	}*/

}
