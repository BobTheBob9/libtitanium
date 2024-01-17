#include "renderer.hpp"
#include "renderer_default_wgsl.hpp"
#include "renderer_stringify.hpp"

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/maths.hpp>
#include <libtitanium/sys/platform_sdl.hpp>
#include <libtitanium/util/static_array.hpp>
#include <libtitanium/logger/logger.hpp>
#include <libtitanium/config/config.hpp>

#include "extern/imgui/imgui.h"
#include "extern/imgui/imgui_impl_wgpu.h"

#include <libtitanium/imgui_widgets/widgets.hpp>

#include <webgpu/webgpu.h>


// TODO: the renderer is super complex and will only get more complex, it should really be split into multiple files
// atm, code is a bit unwieldly, hard to navigate and understand at a glance which sucks

namespace renderer
{
    static bool s_bPreferImmmediatePresent = false; config::Var * pcvarPreferImmediatePresent = config::RegisterVar( "renderer::preferimmediatepresent", config::EFVarUsageFlags::NONE, config::VARF_BOOL, &s_bPreferImmmediatePresent );
    
    void C_WGPUVirtualDeviceHandleUncaughtError( const WGPUErrorType ewgpuErrorType, const char * const pszMessage, void *const pUserdata )
    {
        (void)pUserdata;
        logger::Info( "%s: type: %s, %s" ENDL, __FUNCTION__, WGPUErrorTypeToString( ewgpuErrorType ), pszMessage );
    }

    void ReconfigureSurface( TitaniumPhysicalRenderingDevice *const pRendererDevice, TitaniumRendererState *const pRendererState, const util::maths::Vec2<u32> vWindowSize )
    {
        // get supported surface present modes, ideally take the preferred one, but if that's not available, get the default supported
        const WGPUPresentMode wgpuPresentMode = s_bPreferImmmediatePresent ? WGPUPresentMode_Immediate : WGPUPresentMode_Fifo;
        const WGPUTextureFormat wgpuSwapchainFormat = wgpuSurfaceGetPreferredFormat( pRendererState->m_wgpuRenderSurface, pRendererDevice->m_wgpuGraphicsAdapter );
        const WGPUSurfaceConfiguration wgpuSwapChainDescriptor {
            .device = pRendererState->m_wgpuVirtualDevice,

            .format = wgpuSwapchainFormat,
            .usage = WGPUTextureUsage_RenderAttachment,

            .width = vWindowSize.x,
            .height = vWindowSize.y,

            .presentMode = wgpuPresentMode
        };

        // TODO: this is a bit spammy, add when we have designated spam logs
        //logger::Info( "Creating swapchain: width %i, height %i present mode: %s" ENDL, nWindowWidth, nWindowHeight, WGPUPresentModeToString( wgpuPresentMode ) );
        wgpuSurfaceConfigure( pRendererState->m_wgpuRenderSurface, &wgpuSwapChainDescriptor );
    }

    DepthTextureAndView CreateDepthTextureAndViewForWindowSize( TitaniumRendererState *const pRendererState, const util::maths::Vec2<u32> vWindowSize )
    {
        // make depth buffer
        // TODO: const or unify between depthstencilstate and here somehow
        //
        static const WGPUTextureFormat wgpuDepthTextureFormat = WGPUTextureFormat_Depth24Plus; 
        WGPUTextureDescriptor wgpuDepthTextureDescriptor {
            .usage = WGPUTextureUsage_RenderAttachment,
            .dimension = WGPUTextureDimension_2D,
            .size = { vWindowSize.x, vWindowSize.y, 1 },
            .format = WGPUTextureFormat_Depth24Plus, 
            .mipLevelCount = 1,
            .sampleCount = 1,
            .viewFormatCount = 1,
            .viewFormats = &wgpuDepthTextureFormat
        };

        WGPUTexture wgpuDepthTexture = wgpuDeviceCreateTexture( pRendererState->m_wgpuVirtualDevice, &wgpuDepthTextureDescriptor );
    
        WGPUTextureViewDescriptor wgpuDepthTextureViewDescriptor {
            .format = WGPUTextureFormat_Depth24Plus,
            .dimension = WGPUTextureViewDimension_2D,
            .baseMipLevel = 0,
            .mipLevelCount = 1,
            .baseArrayLayer = 0,
            .arrayLayerCount = 1,
            .aspect = WGPUTextureAspect_DepthOnly
        };
        WGPUTextureView r_wgpuDepthTextureView = wgpuTextureCreateView( wgpuDepthTexture, &wgpuDepthTextureViewDescriptor );

        return {
            .m_wgpuDepthTexture = wgpuDepthTexture, 
            .m_wgpuDepthTextureView = r_wgpuDepthTextureView
        };
    }

    void FreeDepthTextureAndView( DepthTextureAndView *const pDepthTexture )
    {
        wgpuTextureDestroy( pDepthTexture->m_wgpuDepthTexture );
        wgpuTextureRelease( pDepthTexture->m_wgpuDepthTexture );
        wgpuTextureViewRelease( pDepthTexture->m_wgpuDepthTextureView );
    }



    bool Initialise( TitaniumPhysicalRenderingDevice *const pRendererDevice, TitaniumRendererState *const pRendererState, SDL_Window *const psdlWindow )
    {
        logger::Info( "Initialising wgpu renderer..." ENDL );

        WGPUAdapter wgpuGraphicsAdapter = pRendererDevice->m_wgpuGraphicsAdapter;

        const util::maths::Vec2<u32> vWindowSize = sys::sdl::GetWindowSizeVector( psdlWindow );
        WGPUSurface wgpuSurface = pRendererState->m_wgpuRenderSurface = sys::sdl::CreateWGPUSurfaceForWindow( psdlWindow, pRendererDevice->m_wgpuInstance );

        WGPUTextureFormat wgpuSwapchainFormat;
#if WEBGPU_BACKEND_WGPU 
        wgpuSwapchainFormat = wgpuSurfaceGetPreferredFormat( wgpuSurface, wgpuGraphicsAdapter );
#elif WEBGPU_BACKEND_DAWN
        wgpuSwapchainFormat = WGPUTextureFormat_BGRA8Unorm; // dawn only supports this, and doesn't have wgpuSurfaceGetPreferredFormat
#endif 

        // Request virtual graphics device from physical adapter
        WGPUDevice wgpuVirtualDevice = pRendererState->m_wgpuVirtualDevice = [ wgpuGraphicsAdapter ]()
        {
            const WGPUDeviceDescriptor wgpuVirtualDeviceDescriptor { };
            WGPUDevice r_wgpuVirtualDevice;
            wgpuAdapterRequestDevice(
                wgpuGraphicsAdapter,
                &wgpuVirtualDeviceDescriptor,
                []( const WGPURequestDeviceStatus wgpuRequestDeviceStatus, WGPUDevice wgpuVirtualDevice, const char *const pszMessage, void *const pUserdata )
                {
                    logger::Info( "wgpuRequestDeviceStatus returned %s, with message: %s" ENDL, WGPURequestDeviceStatusToString( wgpuRequestDeviceStatus ), pszMessage );
                    
                    if ( wgpuRequestDeviceStatus == WGPURequestDeviceStatus_Success )
                    {
                        *reinterpret_cast<WGPUDevice *>( pUserdata ) = wgpuVirtualDevice;
                    }
                },
                &r_wgpuVirtualDevice
            );

            return r_wgpuVirtualDevice;
        }();

        wgpuDeviceSetUncapturedErrorCallback( wgpuVirtualDevice, C_WGPUVirtualDeviceHandleUncaughtError, &wgpuVirtualDevice );

        pRendererState->m_wgpuQueue = wgpuDeviceGetQueue( wgpuVirtualDevice );
        ReconfigureSurface( pRendererDevice, pRendererState, vWindowSize );

        WGPUSamplerDescriptor wgpuSamplerDesc {
            .magFilter = WGPUFilterMode_Linear,
            .minFilter = WGPUFilterMode_Linear,
            .mipmapFilter = WGPUMipmapFilterMode_Linear,
            .lodMaxClamp = 8.f,
            .maxAnisotropy = 1,
        };
        pRendererState->m_wgpuTextureSampler = wgpuDeviceCreateSampler( wgpuVirtualDevice, &wgpuSamplerDesc );

        ImGui_ImplWGPU_Init( wgpuVirtualDevice, 1, wgpuSwapchainFormat, WGPUTextureFormat_Depth24Plus );


        /*
         *  We're done with webgpu global inits
         *  So, let's start doing stuff specific to our renderer: creating base uniform structures, pipelines and global buffers
         */


        // create error scope for pipeline creation so we can react to it
        wgpuDevicePushErrorScope( wgpuVirtualDevice, WGPUErrorFilter_Validation );

        WGPUBindGroupLayout wgpuRenderViewBindGroupLayout = pRendererState->m_wgpuUniformBindGroupLayout_UShaderView = [ wgpuVirtualDevice ]()
        {
            // create builtin bindgroup layouts, this defines the way uniforms and textures are laid out in the render pipeline
            WGPUBindGroupLayoutEntry wgpuShaderViewBindGroups[] {
                {
                    .binding = 0,
                    .visibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex,
                    .buffer {
                        .type = WGPUBufferBindingType_Uniform,
                        .minBindingSize = sizeof( shaders::UShaderView )
                    }
                }
            };
            WGPUBindGroupLayoutDescriptor wgpuShaderViewBindgroupLayoutDesc {
                .entryCount = util::StaticArray_Length( wgpuShaderViewBindGroups ),
                .entries = wgpuShaderViewBindGroups
            };

            return wgpuDeviceCreateBindGroupLayout( wgpuVirtualDevice, &wgpuShaderViewBindgroupLayoutDesc );
        }();


        WGPUBindGroupLayout wgpuObjectBindGroupLayout = pRendererState->m_wgpuUniformBindGroupLayout_UShaderObjectInstance = [ wgpuVirtualDevice ]()
        {
            // create builtin bindgroup layouts, this defines the way uniforms and textures are laid out in the render pipeline
            WGPUBindGroupLayoutEntry wgpuShaderObjectBindGroups[] {
                // object uniforms
                {
                    .binding = 0,
                    .visibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex,

                    .buffer {
                        .type = WGPUBufferBindingType_Uniform,
                        .minBindingSize = sizeof( shaders::UShaderObjectInstance )
                    }
                },

                // texture and texture sampler
                {
                    .binding = 1,
                    .visibility = WGPUShaderStage_Fragment,
                    .texture {
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_2D
                    }
                },
                {
                    .binding = 2,
                    .visibility = WGPUShaderStage_Fragment,
                    .sampler { .type = WGPUSamplerBindingType_Filtering }
                }
            };
            WGPUBindGroupLayoutDescriptor wgpuShaderViewBindgroupLayoutDesc {
                .entryCount = util::StaticArray_Length( wgpuShaderObjectBindGroups ),
                .entries = wgpuShaderObjectBindGroups
            };

            return wgpuDeviceCreateBindGroupLayout( wgpuVirtualDevice, &wgpuShaderViewBindgroupLayoutDesc );
        }();

        // Create base render pipeline, with a basic shader that uses the global uniform buffer, and the per-object uniform buffer
        pRendererState->m_wgpuObjectRenderPipeline = [ wgpuVirtualDevice, wgpuSwapchainFormat, wgpuRenderViewBindGroupLayout, wgpuObjectBindGroupLayout ]()
        {
            // Create shader module
            WGPUShaderModule wgpuShaderModule = [ wgpuVirtualDevice ]()
            {
                // TODO: need projection matrices in uniforms
                WGPUShaderModuleWGSLDescriptor wgpuShaderCodeDescriptor {
                    .chain { .sType = WGPUSType_ShaderModuleWGSLDescriptor },
                    .code = shaders::DEFAULT_DRAW_3D
                };

                WGPUShaderModuleDescriptor wgpuShaderDescriptor {
                    .nextInChain = &wgpuShaderCodeDescriptor.chain
                };

                return wgpuDeviceCreateShaderModule( wgpuVirtualDevice, &wgpuShaderDescriptor );
            }();

            WGPUBindGroupLayout wgpuPipelineBindGroupLayouts[] {
                wgpuRenderViewBindGroupLayout,
                wgpuObjectBindGroupLayout,
            };

            // create pipeline layout
            WGPUPipelineLayoutDescriptor wgpuPipelineLayoutDescriptor {
                .bindGroupLayoutCount = util::StaticArray_Length( wgpuPipelineBindGroupLayouts ),
                .bindGroupLayouts = wgpuPipelineBindGroupLayouts
            };
            WGPUPipelineLayout wgpuPipelineLayout =  wgpuDeviceCreatePipelineLayout( wgpuVirtualDevice, &wgpuPipelineLayoutDescriptor );

            WGPUVertexAttribute wgpuVertexAttributes[] {
                // position attribute
                {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 0,
                    .shaderLocation = 0
                },
                // uv attribute
                {
                    .format = WGPUVertexFormat_Float32x2,
                    .offset = sizeof( f32 ) * 3,
                    .shaderLocation = 1
                }
            };
            WGPUVertexBufferLayout wgpuVertexBufferLayout {
                .arrayStride = sizeof( f32 ) * 5,
                .stepMode = WGPUVertexStepMode_Vertex,
                .attributeCount = util::StaticArray_Length( wgpuVertexAttributes ),
                .attributes = wgpuVertexAttributes
            };

            WGPUDepthStencilState wgpuDepthStencilOperations {
                .format = WGPUTextureFormat_Depth24Plus,
                .depthWriteEnabled = true,
                .depthCompare = WGPUCompareFunction_Less,

                .stencilFront = { .compare = WGPUCompareFunction_Always },
                .stencilBack = { .compare = WGPUCompareFunction_Always },
                .stencilReadMask = 0xFFFFFFFF,
                .stencilWriteMask = 0xFFFFFFFF
            };

            WGPUBlendState wgpuBlendOperations {
                .color {
                    .operation = WGPUBlendOperation_Add,
                    .srcFactor = WGPUBlendFactor_SrcAlpha,
                    .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha
                }
            };

            WGPUColorTargetState wgpuColourTargetOperations {
                .format = wgpuSwapchainFormat,
                .blend = &wgpuBlendOperations,
                .writeMask = WGPUColorWriteMask_All
            };

            WGPUFragmentState wgpuFragmentOperations {
                .module = wgpuShaderModule,
                .entryPoint = "main_Frag",
                .constantCount = 0,
                .targetCount = 1,
                .targets = &wgpuColourTargetOperations
            };

            WGPURenderPipelineDescriptor wgpuRenderPipelineDescriptor {
                .layout = wgpuPipelineLayout,

                .vertex {
                    .module = wgpuShaderModule,
                    .entryPoint = "main_Vert",
                    .constantCount = 0,
                    .bufferCount = 1,
                    .buffers = &wgpuVertexBufferLayout
                },

                .primitive {
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_Back
                },

                .depthStencil = &wgpuDepthStencilOperations,

                .multisample {
                    .count = 1, // disable multisampling
                    .mask = ~0u,
                    .alphaToCoverageEnabled = false
                },

                .fragment = &wgpuFragmentOperations // this sucks, wish there was a way of aggregate-initialising this inline
            };

            return wgpuDeviceCreateRenderPipeline( wgpuVirtualDevice, &wgpuRenderPipelineDescriptor );
        }();

        bool bPipelineCompilationFailed = false;
        wgpuDevicePopErrorScope( wgpuVirtualDevice, []( const WGPUErrorType ewgpuErrorType, const char * const pszMessage, void *const bPipelineCompilationFailed ){ 
            if ( ewgpuErrorType == WGPUErrorType_NoError )
                return;

            logger::Info( "Pipeline creation failed with message: %s" ENDL, pszMessage );
            *static_cast<bool *>( bPipelineCompilationFailed ) = true;
        }, &bPipelineCompilationFailed );

        // TODO: should this be a part of the RenderView?
        pRendererState->m_depthTextureAndView = CreateDepthTextureAndViewForWindowSize( pRendererState, vWindowSize );
        if ( !bPipelineCompilationFailed )
        {
            logger::Info( "wgpu renderer initialised successfully!" ENDL );
        }

        return !bPipelineCompilationFailed;
    }

	void Shutdown( TitaniumRendererState *const pRendererState )
    {
        wgpuTextureViewRelease( pRendererState->m_depthTextureAndView.m_wgpuDepthTextureView );
        wgpuTextureRelease( pRendererState->m_depthTextureAndView.m_wgpuDepthTexture );
        wgpuSamplerRelease( pRendererState->m_wgpuTextureSampler );

        wgpuBindGroupLayoutRelease( pRendererState->m_wgpuUniformBindGroupLayout_UShaderObjectInstance );
        wgpuBindGroupLayoutRelease( pRendererState->m_wgpuUniformBindGroupLayout_UShaderView );
        wgpuRenderPipelineRelease( pRendererState->m_wgpuObjectRenderPipeline );

        wgpuSurfaceRelease( pRendererState->m_wgpuRenderSurface );

        wgpuQueueRelease( pRendererState->m_wgpuQueue );

        wgpuDeviceRelease( pRendererState->m_wgpuVirtualDevice );
        wgpuDeviceDestroy( pRendererState->m_wgpuVirtualDevice );
    }

    // TODO: the fact that this takes a renderview is weird (cont)
    // i think in an ideal world, we would have a separate concept of a render target with a given size, and a view which controls camera transforms and stuff
    // though, in practice i don't see a situation where code would ever really reuse a single view with multiple targets, so maybe views own targets? idk
    void ResolutionChanged( TitaniumPhysicalRenderingDevice *const pRendererDevice, TitaniumRendererState *const pRendererState, RenderView *const pRenderView, const util::maths::Vec2<u32> vWindowSize )
    {
        pRenderView->m_vRenderResolution = vWindowSize;
        pRenderView->m_bGPUDirty = true;

        ReconfigureSurface( pRendererDevice, pRendererState, vWindowSize );

        FreeDepthTextureAndView( &pRendererState->m_depthTextureAndView );
        pRendererState->m_depthTextureAndView = CreateDepthTextureAndViewForWindowSize( pRendererState, vWindowSize );
    }

    void Preframe_ImGUI()
    {
        ImGui_ImplWGPU_NewFrame();
    }



    void Frame( TitaniumRendererState *const pRendererState, RenderView *const pRenderView, const util::data::Span<RenderObject> sRenderObjects )
    {
#if WEBGPU_BACKEND_WGPU 
        // TODO: what do i actually need here??
        //wgpuDevicePoll( pRendererState->m_wgpuVirtualDevice );
#elif WEBGPU_BACKEND_DAWN
        wgpuDeviceTick( pRendererState->m_wgpuVirtualDevice );
#endif 

        // write view state to view uniform if view state has changed
        if ( pRenderView->m_bGPUDirty )
        {
            RenderView::WriteToUniformBuffer( pRendererState, pRenderView );
        }

        ImGui::Render();

        WGPUSurfaceTexture wgpuSurfaceTexture;
        wgpuSurfaceGetCurrentTexture( pRendererState->m_wgpuRenderSurface, &wgpuSurfaceTexture );

        WGPUTextureView wgpuNextTexture = wgpuTextureCreateView( wgpuSurfaceTexture.texture, nullptr );

        WGPUCommandEncoderDescriptor wgpuCommandEncoderDescriptor {};
        WGPUCommandEncoder wgpuCommandEncoder = wgpuDeviceCreateCommandEncoder( pRendererState->m_wgpuVirtualDevice, &wgpuCommandEncoderDescriptor );
        {
            // describe a render pass
            WGPURenderPassColorAttachment wgpuRenderPassColourAttachment {
                .view = wgpuNextTexture,
                .loadOp = WGPULoadOp_Clear,
                .storeOp = WGPUStoreOp_Store,
                .clearValue { 0.05, 0.05, 0.05, 1.0 },
            };

            WGPURenderPassDepthStencilAttachment wgpuRenderPassDepthStencilAttachment {
                .view = pRendererState->m_depthTextureAndView.m_wgpuDepthTextureView,
                .depthLoadOp = WGPULoadOp_Clear,
                .depthStoreOp = WGPUStoreOp_Store,
                .depthClearValue = 1.0f,
                .depthReadOnly = false,

                // wgpu and dawn disagree here if stencil isn't in use
                // dawn wants these to be undefined (0) in this case, wgpu will complain if they're ever undefined
#if WEBGPU_BACKEND_WGPU
                .stencilLoadOp = WGPULoadOp_Clear,
                .stencilStoreOp = WGPUStoreOp_Store,
#endif
                .stencilReadOnly = false
            };

            WGPURenderPassDescriptor wgpuRenderPassDescriptor {
                .colorAttachmentCount = 1,
                .colorAttachments = &wgpuRenderPassColourAttachment,
                .depthStencilAttachment = &wgpuRenderPassDepthStencilAttachment
            };

            WGPURenderPassEncoder wgpuRenderPass = wgpuCommandEncoderBeginRenderPass( wgpuCommandEncoder, &wgpuRenderPassDescriptor );
            {
                // Select render pipeline
                // TODO: only support 1 render pipeline/bindgroup atm, should support more at some point!
                wgpuRenderPassEncoderSetPipeline( wgpuRenderPass, pRendererState->m_wgpuObjectRenderPipeline );
                wgpuRenderPassEncoderSetBindGroup( wgpuRenderPass, shaders::EBindgroup::RENDERVIEW, pRenderView->m_viewUniforms.m_wgpuBindGroup, 0, nullptr );

                for ( uint i = 0; i < sRenderObjects.nLength; i++ )
                {
                    // write object state to object uniform if object state has changed
                    RenderObject *const pRenderObject = &sRenderObjects.pData[ i ];
                    if ( pRenderObject->m_bGPUDirty ) // do we need to update the object's state on the gpu?
                    {
                        RenderObject::WriteToUniformBuffer( pRendererState, pRenderObject );
                    }

                    // set current object and render
                    wgpuRenderPassEncoderSetBindGroup( wgpuRenderPass, shaders::EBindgroup::RENDEROBJECT, pRenderObject->m_objectUniforms.m_wgpuBindGroup, 0, nullptr );
                    wgpuRenderPassEncoderSetVertexBuffer( wgpuRenderPass, 0, pRenderObject->m_gpuModel.m_wgpuVertexBuffer, 0, sRenderObjects.pData[ i ].m_gpuModel.m_nVertexBufferSize );
                    wgpuRenderPassEncoderSetIndexBuffer( wgpuRenderPass, pRenderObject->m_gpuModel.m_wgpuIndexBuffer, WGPUIndexFormat_Uint16, 0, sRenderObjects.pData[ i     ].m_gpuModel.m_nIndexBufferSize );
                    wgpuRenderPassEncoderDrawIndexed( wgpuRenderPass, pRenderObject->m_gpuModel.m_nIndexBufferCount, 1, 0, 0, 0 );
                }

                // imgui
                ImGui_ImplWGPU_RenderDrawData( ImGui::GetDrawData(), wgpuRenderPass );
            }
            wgpuRenderPassEncoderEnd( wgpuRenderPass );
            wgpuRenderPassEncoderRelease( wgpuRenderPass );
        }
        WGPUCommandBufferDescriptor wgpuCommandBufferDescriptor {};
        WGPUCommandBuffer wgpuCommand = wgpuCommandEncoderFinish( wgpuCommandEncoder, &wgpuCommandBufferDescriptor );
        wgpuCommandEncoderRelease( wgpuCommandEncoder );

        wgpuTextureViewRelease( wgpuNextTexture );

        wgpuQueueSubmit( pRendererState->m_wgpuQueue, 1, &wgpuCommand );
        wgpuSurfacePresent( pRendererState->m_wgpuRenderSurface );
    }
}
