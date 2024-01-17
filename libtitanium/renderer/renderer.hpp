#pragma once

// TODO temp, ideally this should be in CMakeLists.txt, and probably used across the codebase (even game code!)
#define TITANIUM_SDL 1

#include <webgpu/webgpu.h>
#if TITANIUM_SDL
	#include <SDL.h>
#endif // #if TITANIUM_SDL
#include <imgui.h>

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

	/*
	 * 	A GPUFunction is a compiled function that tells the GPU how to draw an object
	 * 	Each function takes a constant set of inputs and returns outputs, just like a normal function!
	 * 		Currently, this is limited to just a combined vertex and fragment shader, compiled as one
	 * 		In the future, things like compute shaders might be supported too!!
	 */
	struct GPUFunction
	{
		struct
		{

		} internal;

		static GPUFunction Create();
	};

	/*
	 *	A RenderObject is a complete, renderable Thing, pulling together lots of resources and data.
	 * 		A RenderObject is made of both references to resources (e.g. the object's model, textures, shader program to draw it with)
	 * 		As well as data to tell it how to draw those resources (e.g. position, rotation, additional uniform vars, etc)
	 */
	struct RenderObject
	{
		struct {
			GPUFunction * pDrawFunction;
			GPUModel * pModel;
			GPUTexture * pTexture;
		} resource;

		util::maths::Vec3<f32> position;
		util::maths::Vec3Angle<f32> rotation;
	};

	/*
	 * 	A renderer::View is the point of view from which a collection of RenderObjects are rendered
	 * 		Essentially, it can be thought of as a camera, it's got a position, rotation and field of view
	 */
	struct View
	{
		util::maths::Vec3<f32> position;
		util::maths::Vec3Angle<f32> rotation;
		f32 fov;
	};

	/*
	 * 	RenderSettings is a simple way of turning different parts of the rendering process on and off
	 */
	struct RenderSettings
	{
		bool bClearBackground = false;
	};

	void RenderViewAndObjectsToTarget( Device *const pDevice, View *const pView, util::data::Span<RenderObject> sRenderObjects, DrawTarget *const pDrawTarget, RenderSettings settings );
};
