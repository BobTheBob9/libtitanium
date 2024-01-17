#include "renderer.hpp"

#include <libtitanium/util/assert.hpp>

#include <webgpu/webgpu.h>
#include <imgui.h>
#include <imgui_impl_wgpu.h>

namespace renderer
{
	void RenderViewAndObjectsToTarget( Device *const pDevice, View *const pView, util::data::Span<RenderObject> sRenderObjects, DrawTarget *const pDrawTarget, RenderSettings settings )
	{
		(void)pView;
		(void)sRenderObjects;

		//wgpuDevicePoll
		// why is wgpuDevicePoll vs wgpuDeviceTick not standardised yet??? bleh

		ImGui::Render();

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
				.clearValue = { 0.05f, 0.05f, 0.05f, 1.f }
			};

			WGPURenderPassDescriptor wgpuRenderPassDescriptor = {
				.colorAttachmentCount = settings.bClearBackground ? 1ull : 0ull,
				.colorAttachments = &wgpuRenderPassClearColourDescriptor,
				.depthStencilAttachment  = &wgpuRenderPassDepthStencilDescriptor
			};

			WGPURenderPassEncoder wgpuRenderPass = wgpuCommandEncoderBeginRenderPass( wgpuCommandEncoder, &wgpuRenderPassDescriptor );
			{
				// TODO: render objects, etcetera

				ImGui_ImplWGPU_RenderDrawData( ImGui::GetDrawData(), wgpuRenderPass );
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
}
