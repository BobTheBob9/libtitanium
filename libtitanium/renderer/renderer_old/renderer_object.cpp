#include "renderer.hpp"
#include "renderer_default_wgsl.hpp"

#include <libtitanium/logger/logger.hpp>
#include <libtitanium/util/maths.hpp>
#include <libtitanium/util/static_array.hpp>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace renderer
{
    void RenderView::Create( TitaniumRendererState *const pRendererState, RenderView *const pRenderView )
    {
        // make buffer
        WGPUBufferDescriptor wgpuUniformBufferDescriptor {
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
            .size = sizeof( shaders::UShaderView )
        };
        WGPUBuffer wgpuUniformBuffer = wgpuDeviceCreateBuffer( pRendererState->m_wgpuVirtualDevice, &wgpuUniformBufferDescriptor );

        // make binding to buffer
        WGPUBindGroupEntry wgpuBinding {
            .binding = 0,
            .buffer = wgpuUniformBuffer,
            .offset = 0,
            .size = sizeof( shaders::UShaderView )
        };
        WGPUBindGroupDescriptor wgpuBindGroupDescriptor {
            .layout = pRendererState->m_wgpuUniformBindGroupLayout_UShaderView,
            .entryCount = 1, // keep same as wgpuBindGroupLayoutDescriptor.entryCount!
            .entries = &wgpuBinding
        };
        WGPUBindGroup r_wgpuBindGroup = wgpuDeviceCreateBindGroup( pRendererState->m_wgpuVirtualDevice, &wgpuBindGroupDescriptor );

        pRenderView->m_viewUniforms = { .m_wgpuBindGroup = r_wgpuBindGroup, .m_wgpuBuffer = wgpuUniformBuffer };
        WriteToUniformBuffer( pRendererState, pRenderView );
    }

	void RenderView::Free( RenderView *const pRenderView )
    {
       wgpuBufferDestroy( pRenderView->m_viewUniforms.m_wgpuBuffer );
       wgpuBufferRelease( pRenderView->m_viewUniforms.m_wgpuBuffer );
       wgpuBindGroupRelease( pRenderView->m_viewUniforms.m_wgpuBindGroup );
    }

    void RenderView::WriteToUniformBuffer( TitaniumRendererState *const pRendererState, RenderView *const pRenderView )
    {
        shaders::UShaderView uView {
            .mat4fViewTransform = util::maths::Matrix4x4::FromViewPositionAngles( pRenderView->m_vCameraPosition, util::maths::Vec3Angle<float>::MultiplyScalar( pRenderView->m_vCameraRotation, util::maths::DEG_TO_RAD<float> ) ),
            .mat4fPerspective = util::maths::Matrix4x4::FromProjectionPerspective( pRenderView->m_vRenderResolution, pRenderView->m_flCameraFOV, 0.01, 10000.0 )
        };

        wgpuQueueWriteBuffer( pRendererState->m_wgpuQueue, pRenderView->m_viewUniforms.m_wgpuBuffer, 0, &uView, sizeof( uView ) );
        pRenderView->m_bGPUDirty = false;
    }



    void RenderObject::Create( TitaniumRendererState *const pRendererState, RenderObject *const pRenderObject )
    {
        // make buffer
        WGPUBufferDescriptor wgpuStandardUniformBufferDescriptor {
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
            .size = sizeof( shaders::UShaderObjectInstance )
        };
        WGPUBuffer wgpuUniformBuffer = wgpuDeviceCreateBuffer( pRendererState->m_wgpuVirtualDevice, &wgpuStandardUniformBufferDescriptor );

        // make binding to buffer
        WGPUBindGroupEntry wgpuObjectBindings[] {
            {
                .binding = 0,
                .buffer = wgpuUniformBuffer,
                .size = sizeof( shaders::UShaderObjectInstance )
            },
            {
                .binding = 1,
                .textureView = pRenderObject->m_gpuTexture.m_wgpuTextureView
            },
            {
                .binding = 2,
                .sampler = pRendererState->m_wgpuTextureSampler
            }
        };

        WGPUBindGroupDescriptor wgpuBindGroupDescriptor {
            .layout = pRendererState->m_wgpuUniformBindGroupLayout_UShaderObjectInstance,
            .entryCount = util::StaticArray_Length( wgpuObjectBindings ),
            .entries = wgpuObjectBindings
        };
        WGPUBindGroup r_wgpuBindGroup = wgpuDeviceCreateBindGroup( pRendererState->m_wgpuVirtualDevice, &wgpuBindGroupDescriptor );

        pRenderObject->m_objectUniforms = { .m_wgpuBindGroup = r_wgpuBindGroup, .m_wgpuBuffer = wgpuUniformBuffer };
        WriteToUniformBuffer( pRendererState, pRenderObject );
    }

    void RenderObject::Free( RenderObject *const pRenderObject )
    {
        wgpuBufferDestroy( pRenderObject->m_objectUniforms.m_wgpuBuffer );
        wgpuBufferRelease( pRenderObject->m_objectUniforms.m_wgpuBuffer );
        wgpuBindGroupRelease( pRenderObject->m_objectUniforms.m_wgpuBindGroup );
    }

    void RenderObject::WriteToUniformBuffer( TitaniumRendererState *const pRendererState, RenderObject *const pRenderObject )
    {
        shaders::UShaderObjectInstance uObject {
            .mat4fBaseTransform = util::maths::Matrix4x4::FromPositionAngles( pRenderObject->m_vPosition, util::maths::Vec3Angle<float>::MultiplyScalar( pRenderObject->m_vRotation, util::maths::DEG_TO_RAD<float> ) )
        };

        wgpuQueueWriteBuffer( pRendererState->m_wgpuQueue, pRenderObject->m_objectUniforms.m_wgpuBuffer, 0, &uObject, sizeof( uObject ) );
        pRenderObject->m_bGPUDirty = false; // gpu has up-to-date state for the object
    }
}
