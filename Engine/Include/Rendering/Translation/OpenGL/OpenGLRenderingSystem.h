#if defined(CATALYST_RENDERING_OPENGL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Pointers/UniquePointer.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Forward declarations.
class RenderPass;
class Resolution;
class TextureData;

class OpenGLRenderingSystem final
{

public:

	//Singletone declaration.
	DECLARE_SINGLETON(OpenGLRenderingSystem);

	/*
	*	Default constructor.
	*/
	OpenGLRenderingSystem() NOEXCEPT { }

	/*
	*	Pre-initializes the OpenGL rendering system.
	*/
	void PreInitializeSystem() NOEXCEPT;

	/*
	*	Post-initializes the OpenGL rendering system.
	*/
	void PostInitializeSystem() NOEXCEPT;

	/*
	*	Pre-updates the OpenGL rendering system synchronously.
	*/
	void PreUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Post-updates the OpenGL rendering system synchronously.
	*/
	void PostUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the OpenGL rendering system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the number of frame buffers
	*/
	uint8 GetNumberOfFrameBuffers() const NOEXCEPT;

	/*
	*	Returns the current frame buffer index.
	*/
	uint8 GetCurrentFrameBufferIndex() const NOEXCEPT;

	/*
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Destroys a texture 2D
	*/
	void DestroyTexture2D(Texture2DHandle handle) NOEXCEPT;

	/*
	*	Creates a texture cube.
	*/
	TextureCubeHandle CreateTextureCube(const float *const RESTRICT data, const Resolution resolution) const NOEXCEPT;

	/*
	*	Creates a buffer.
	*/
	ConstantBufferHandle CreateBuffer(const uint64 size) const NOEXCEPT;

	/*
	*	Uploads data to a buffer.
	*/
	void UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT dataSizes, const uint8 dataChunks, ConstantBufferHandle handle) const NOEXCEPT;

	/*
	*	Destroys a constant buffer.
	*/
	void DestroyConstantBuffer(ConstantBufferHandle handle) NOEXCEPT;

	/*
	*	Creates a depth buffer.
	*/
	DepthBufferHandle CreateDepthBuffer(const Resolution resolution) const NOEXCEPT;

	/*
	*	Creates a render data table layout.
	*/
	void CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 numberOfBindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a render data table.
	*/
	void CreateRenderDataTable(const RenderDataTableLayoutHandle renderDataTableLayout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Binds a combined image sampler to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	void BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT;

	/*
*	Binds a sampled image to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
	void BindSampledImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, OpaqueHandle image) const NOEXCEPT;

	/*
	*	Binds a sampler to a render data table.
	*/
	void BindSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, SamplerHandle sampler) const NOEXCEPT;

	/*
	*	Binds a uniform buffer to a render data table.
	*/
	void BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, UniformBufferHandle uniformBuffer) const NOEXCEPT;

	/*
	*	Destroys a render data table.
	*/
	void DestroyRenderDataTable(RenderDataTableHandle handle) NOEXCEPT;

	/*
	*	Creates a render target.
	*/
	void CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates and returns a sampler.
	*/
	SamplerHandle CreateSampler(const SamplerProperties &properties) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize, const BufferUsage usage) const NOEXCEPT;

	/*
	*	Uploads data to a uniform buffer.
	*/
	void UploadDataToUniformBuffer(UniformBufferHandle handle, const void *const RESTRICT data) const NOEXCEPT;

	/*
	*	Destroys a uniform buffer.
	*/
	void DestroyUniformBuffer(UniformBufferHandle handle) NOEXCEPT;

	/*
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeRenderPassInitialization(RenderPass *const RESTRICT _RenderPass) NOEXCEPT;

};
#endif