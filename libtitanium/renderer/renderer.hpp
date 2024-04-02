#pragma once

#include <webgpu/webgpu.h>
#if TITANIUM_SDL
	#include <SDL.h>
#endif // #if TITANIUM_SDL

#include <libtitanium/util/maths.hpp>
#include <libtitanium/util/data/span.hpp>

namespace renderer
{
	/*
	 * 	A renderer::Device is our handle to the actual GPU we're rendering on, so we can upload stuff to it, and ask it to render things!
	 */
	struct Device
	{
		struct
		{
			WGPUInstance wgpuInstance;
			WGPUAdapter wgpuGraphicsDevice;

			WGPUDevice wgpuVirtualDevice;

			WGPUQueue wgpuDeviceQueue;
		} internal;

		static bool Create( Device *const o_renderDevice );
		static void Destroy( Device *const pDevice );
	};

	/*
	 * 	A renderer::DrawTarget is something that can be rendered to, like a window, or a texture
	 * 		(though, currently, only windows are supported!)
	 */
	struct DrawTarget
	{
		// TODO: will likely need a rework if/when we ever support nested views/rendertextures
		struct
		{
			WGPUSurface wgpuTargetSurface;
			WGPUTextureFormat wgpuTargetSurfaceTextureFormat;

			WGPUTexture wgpuDepthTexture;
			WGPUTextureView wgpuDepthTextureView;
		} internal;

#if TITANIUM_SDL
		static DrawTarget CreateFromSysWindow_SDL( Device *const pRenderDevice, SDL_Window *const psdlWindow );
		static void RecreateFromSysWindow_SDL( DrawTarget *const pDrawTarget, Device *const pRenderDevice, SDL_Window *const psdlWindow );
#endif // #if TITANIUM_SDL

		static void Destroy( DrawTarget *const pTarget );
	};

	/*
	 * 	The "GPU" prefix denotes objects that are simply handles to objects that exist in GPU memory
	 * 	For instance GPUTexture doesn't store any actual textures, it just stores a handle to the texture that exists on the GPU, for use on the GPU
	 */
	struct GPUTexture
	{
		struct
		{
			WGPUTextureView wgpuTextureView;
			WGPUTexture wgpuTexture;
		} internal;
	};

	/*
	 * 	A GPUModel is a handle to a buffer on the GPU containing vertex and index buffers with all the necessary attributes for UVs and such
	 */
	struct GPUModel
	{
		struct
		{

		} internal;

	};

	// TODO: probably analyse whether GPUFunction stuff should handle both compute and draw, or whether these should be separate objects

	// usage: struct SHADER_UNIFORM SomeStruct {};
	// Intended for marshalling data from cpu => gpu, shouldn't be used outside of this for performance reasons
	#define SHADER_UNIFORM alignas( 16 )

	/*
	 * 	A GPUFunction is a compiled function that tells the GPU how to draw an object
	 * 	Each function takes a constant set of inputs and returns outputs, just like a normal function!
	 * 		Currently, this is limited to just a combined vertex and fragment shader, compiled as one
	 * 		In the future, things like compute shaders might be supported too!!
	 *
	 *
	 * 	TODO: this is horribly wasteful currently, no way to recompile a single shader with different consts, share bindgroups across shaders, etc
	 */
	struct GPUFunction
	{
		struct CreationResult
		{
			bool success;

			int column, row; // TODO: if compilation failed, then error info, line numbers, etc if available
			char szMessage[ 2048 ]; // TODO: expensive!! maybe replace with pointer or handle to runtime allocated buffer when we have good temporary/local allocators
		};

		struct
		{
			WGPUBindGroupLayout wgpuFunctionUniformBindGroup; // defines uniform buffers need to be laid out for the function's caller
			WGPUBindGroupLayout wgpuFunctionSamplerBindGroup;
			WGPUBindGroupLayout wgpuFunctionTextureBindGroup;
			WGPURenderPipeline wgpuRenderPipeline;
		} internal;

		// TODO: kinda sucks this has to rely on the drawtarget for colour information, maybe there's a better way of doing this?
		// also means we (theoretically) need a shader recompile if we ever recreate the drawtarget as it could change colour info, bleh
		static CreationResult CreateFromSource_WGSL( Device *const pRenderDevice, DrawTarget *const pDrawTarget, GPUFunction *const o_func, util::data::Span<size_t> shaderArgSizes, uint numTextures, const char *const pszShaderSourceWGSL );

		// TODO: could expand this with stuff like CreateFromSource_SPIRV, or even CreateFromSource_ASM for platforms where we want to ship precompiled shaders
		// though, the latter would require a custom webgpu extension i believe, if we'd be sticking with webgpu on those platforms at all?
	};


	/*
	 * 	Arguments passed to the call of a GPUFunction
	 * 		Currently, this is assuming meshes n stuff are required by every function call, perhaps a bad thing?
	 */
	struct GPUFunctionArgs
	{
		// TODO: mostly adding for dev testing, possibly remove
		enum class eMeshType
		{
			VERTEX,
			VERTEX_INDEX
		} meshType = eMeshType::VERTEX_INDEX;

		struct
		{
			// TODO: support multiple uniforms
			WGPUBindGroup wgpuUniformBindGroup; WGPUBuffer wgpuUniformBuffer;
			WGPUBindGroup wgpuTextureSamplerBindGroup; WGPUSampler wgpuTextureSampler;

			WGPUBuffer wgpuVertexBuffer; // contains both vertices and indices, vertices at the start
			size_t vertexBufSize; size_t vertexBufNumVertices; // TODO: adding vertexBufNumVertices for dev testing purposes

			size_t indexBufOffset; // the offset into wgpuVertexBuffer where the indices begin
			size_t indexBufSize; size_t indexBufNumIndices;
		} internal;
	};

	struct DrawCommand
	{
		//util::data::MultiTree<DrawCommand> subCommands; // this needs fleshing out, but this is intended primarily to allow rendertextures n stuff: so a sub-command could render a scene to a screen, which is then rendered as part of the main scene

		GPUFunction * gpuFunction; // shader function
		util::data::Span<GPUFunctionArgs> callArgs; // arguments to each call of the function, the gpu func will be called once for each argument struct provided
	};

	void RunDrawCommands( Device *const pDevice, DrawTarget *const pDrawTarget, const util::data::Span<DrawCommand> drawCommands );
};
