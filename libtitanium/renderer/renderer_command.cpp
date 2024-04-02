#include "renderer.hpp"

#include <libtitanium/memory/mem_core.hpp>
#include <libtitanium/util/defer.hpp>
#include <libtitanium/util/static_array.hpp>
#include <libtitanium/util/data/span.hpp>

#include <webgpu/webgpu.h>

namespace renderer
{
    void HandlePipelineCompilationFail( const WGPUErrorType errorType, const char *const pszMessage, void *const pCompilationError )
    {
        if ( errorType == WGPUErrorType_NoError )
            return;

        GPUFunction::CreationResult *const pCompilationResult = static_cast<GPUFunction::CreationResult *const>( pCompilationError );
        pCompilationResult->success = false;
        // TODO: replace when we have better string funcs
        strncpy( pCompilationResult->szMessage, pszMessage, sizeof( pCompilationResult->szMessage ) );
        pCompilationResult->szMessage[ sizeof( pCompilationResult->szMessage ) - 1 ] = '\0'; // null term for strncpy
    }

    GPUFunction::CreationResult GPUFunction::CreateFromSource_WGSL( Device *const pRenderDevice, DrawTarget *const pDrawTarget, GPUFunction *const o_func, util::data::Span<size_t> shaderArgSizes, uint numTextures, const char *const pszShaderSourceWGSL )
    {
        // This is heavily commented as i find alot of the calls here to be not explain themselves particularly well by name
        // perhaps to someone more familiar with gpu code this is overcommented, for me it's just right

        // wgpuDevicePush/PopErrorScope basically acts as try/catch blocks for webgpu
        // the push call is our try { }, the pop call is our catch { }, and executes a callback as the body
        // we use this to determine if we successfully compiled our code on the gpu, and return accordingly
        wgpuDevicePushErrorScope( pRenderDevice->internal.wgpuVirtualDevice, WGPUErrorFilter_Validation );
        {
            // Create shader module, i'm unsure how exactly this is defined in the spec but i'm relatively certain this compiles wgsl->spirv on the cpu and adds some metadata for functions and such
            WGPUShaderModuleWGSLDescriptor wgpuShaderCodeDescriptor = {
                .chain = { .sType = WGPUSType_ShaderModuleWGSLDescriptor },
                .code = pszShaderSourceWGSL
            };
            WGPUShaderModuleDescriptor wgpuShaderDescriptor = { .nextInChain = &wgpuShaderCodeDescriptor.chain };
            WGPUShaderModule wgpuShaderModule = wgpuDeviceCreateShaderModule( pRenderDevice->internal.wgpuVirtualDevice, &wgpuShaderDescriptor );

            // Create bindgroup layouts, this controls the way uniform data is mapped onto gpu memory
            // First, the ones for uniforms
            WGPUBindGroupLayoutEntry * pwgpuUniformBindGroupDefDescriptorEntries = nullptr; defer( if ( pwgpuUniformBindGroupDefDescriptorEntries ) memory::free( pwgpuUniformBindGroupDefDescriptorEntries ) );
            if ( shaderArgSizes.nLength )
            {
                pwgpuUniformBindGroupDefDescriptorEntries = memory::alloc_nT<WGPUBindGroupLayoutEntry>( shaderArgSizes.nLength );

                for ( uint i = 0; i < shaderArgSizes.nLength; i++ )
                {
                    pwgpuUniformBindGroupDefDescriptorEntries[ i ] = {
                        .binding = i,
                        .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
                        .buffer = {
                            .type = WGPUBufferBindingType_Uniform,
                            .minBindingSize = shaderArgSizes.pData[ i ]
                        }
                    };
                }

                // TODO: arguably sucks
                WGPUBindGroupLayoutDescriptor wgpuUniformBindGroupDefDescriptor = { .entryCount = shaderArgSizes.nLength, .entries = pwgpuUniformBindGroupDefDescriptorEntries };
                o_func->internal.wgpuFunctionUniformBindGroup = wgpuDeviceCreateBindGroupLayout( pRenderDevice->internal.wgpuVirtualDevice, &wgpuUniformBindGroupDefDescriptor );
            }

            WGPUBindGroupLayoutEntry * pwgpuTextureBindGroupDefDescriptorEntries = nullptr; defer( if ( pwgpuTextureBindGroupDefDescriptorEntries ) memory::free( pwgpuTextureBindGroupDefDescriptorEntries ) );
            bool hasTextures = numTextures;
            if ( hasTextures )
            {
                // Next, bind the texture sampler
                // TODO: let caller decide params for the sampler here
                WGPUBindGroupLayoutEntry wgpuSamplerBindGroupDefDescriptorEntry = { .binding = 0, .visibility = WGPUShaderStage_Fragment, .sampler = { .type = WGPUSamplerBindingType_Filtering } };
                WGPUBindGroupLayoutDescriptor wgpuSamplerBindGroupDefDescriptor = { .entryCount = 1, .entries = &wgpuSamplerBindGroupDefDescriptorEntry };
                o_func->internal.wgpuFunctionSamplerBindGroup = wgpuDeviceCreateBindGroupLayout( pRenderDevice->internal.wgpuVirtualDevice, &wgpuSamplerBindGroupDefDescriptor );

                // Finally, bind the textures
                // TODO: one texture format only atm :c
                pwgpuTextureBindGroupDefDescriptorEntries = memory::alloc_nT<WGPUBindGroupLayoutEntry>( numTextures );
                for ( uint i = 0; i < numTextures; i++ )
                {
                    pwgpuTextureBindGroupDefDescriptorEntries[ i ] = {
                        .binding = i,
                        .visibility = WGPUShaderStage_Fragment,
                        .texture = {
                            .sampleType = WGPUTextureSampleType_Float,
                            .viewDimension = WGPUTextureViewDimension_2D
                        }
                    };
                }

                WGPUBindGroupLayoutDescriptor wgpuTextureBindGroupDefDescriptor = { .entryCount = numTextures, .entries = pwgpuTextureBindGroupDefDescriptorEntries };
                o_func->internal.wgpuFunctionTextureBindGroup = wgpuDeviceCreateBindGroupLayout( pRenderDevice->internal.wgpuVirtualDevice, &wgpuTextureBindGroupDefDescriptor );
            }

            // Create the pipeline, i.e. the complete assembled shader function that we can call on the gpu to render us a triangle or a pixel or something
            // bindgroup defs ( map the stuff we defined above to the pipeline )
            constexpr uint UNIFORM_LAYOUTS = 1;
            constexpr uint TEXTURE_LAYOUTS = 2;
            constexpr uint MAX_LAYOUTS = UNIFORM_LAYOUTS + TEXTURE_LAYOUTS;
            WGPUBindGroupLayout bindGroupLayouts[ MAX_LAYOUTS ] = { o_func->internal.wgpuFunctionUniformBindGroup, o_func->internal.wgpuFunctionSamplerBindGroup, o_func->internal.wgpuFunctionTextureBindGroup };
            WGPUPipelineLayoutDescriptor wgpuPipelineLayoutDescriptor = { .bindGroupLayoutCount = UNIFORM_LAYOUTS + ( hasTextures ? TEXTURE_LAYOUTS : 0 ), .bindGroupLayouts = bindGroupLayouts };
            WGPUPipelineLayout wgpuPipelineLayout = wgpuDeviceCreatePipelineLayout( pRenderDevice->internal.wgpuVirtualDevice, &wgpuPipelineLayoutDescriptor );

            // Input model vertex data layout and triangle shader definition
            // The triangle shader (normally a vertex shader, but i prefer triangle) basically defines where all the triangles we send to the gpu are mapped out in screen space (so { 0-1, 0-1 }, where 0=top/left, and 1=bottom/right )
            WGPUVertexAttribute wgpuVertexDataLayout[] = {
                // position
                {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 0,
                    .shaderLocation = 0
                },
            };
            WGPUVertexBufferLayout wgpuVertexBufferLayout = {
                .arrayStride = sizeof( f32 ) * 3,
                .stepMode = WGPUVertexStepMode_Vertex,
                .attributeCount = util::StaticArray_Length( wgpuVertexDataLayout ),
                .attributes = wgpuVertexDataLayout
            };
            WGPUVertexState wgpuTriangleShaderOps = {
                .module = wgpuShaderModule,
                .entryPoint = "triangle",
                .constantCount = 0,
                .bufferCount = 1,
                .buffers = &wgpuVertexBufferLayout
            };

            WGPUDepthStencilState wgpuDepthStencilOps = {
                .format = WGPUTextureFormat_Depth24Plus,
                .depthWriteEnabled = true,
                .depthCompare = WGPUCompareFunction_Less,

                .stencilFront = { .compare = WGPUCompareFunction_Always },
                .stencilBack = { .compare = WGPUCompareFunction_Always },
                .stencilReadMask = 0xFFFFFFFF,
                .stencilWriteMask = 0xFFFFFFFF
            };

            // Pixel shader definition and output parameters
            // The pixel shader consumes the triangles output from our triangle shader and colours pixels of the screen that contain said triangles (i.e. it renders them to the screen!)
            #define colour color // petty
            WGPUBlendState wgpuColourBlendOps = {
                .colour {
                    .operation = WGPUBlendOperation_Add,
                    .srcFactor = WGPUBlendFactor_SrcAlpha,
                    .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha
                }
            };
            #undef colour
            WGPUColorTargetState wgpuColourTargetOps = {
                .format = pDrawTarget->internal.wgpuTargetSurfaceTextureFormat,
                .blend = &wgpuColourBlendOps,
                .writeMask = WGPUColorWriteMask_All
            };
            WGPUFragmentState wgpuPixelShaderOps = {
                .module = wgpuShaderModule,
                .entryPoint = "pixel",
                .constantCount = 0,
                .targetCount = 1,
                .targets = &wgpuColourTargetOps
            };

            // we've got everything, finally make the pipeline
            WGPURenderPipelineDescriptor wgpuRenderPipelineDescriptor = {
                .layout = wgpuPipelineLayout,

                .vertex = wgpuTriangleShaderOps,
                .primitive = { .topology = WGPUPrimitiveTopology_TriangleList, .frontFace = WGPUFrontFace_CCW, .cullMode = WGPUCullMode_Back },
                .depthStencil = &wgpuDepthStencilOps,

                .multisample = { .count = 1, .mask = ~0u },
                .fragment = &wgpuPixelShaderOps
            };
            o_func->internal.wgpuRenderPipeline = wgpuDeviceCreateRenderPipeline( pRenderDevice->internal.wgpuVirtualDevice, &wgpuRenderPipelineDescriptor );
        }
        GPUFunction::CreationResult compileResult = { .success = true }; // populated by the HandlePipelineCompilationFail callback
        wgpuDevicePopErrorScope( pRenderDevice->internal.wgpuVirtualDevice, HandlePipelineCompilationFail, (void *)&compileResult );

        return compileResult;
    }
}
