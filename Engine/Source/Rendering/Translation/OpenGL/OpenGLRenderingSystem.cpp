#if defined(CATALYST_RENDERING_OPENGL)
//Header file.
#include <Rendering/Translation/OpenGL/OpenGLRenderingSystem.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/Resolution.h>
#include <Rendering/Engine/RenderPasses/RenderPass.h>

//Singleton definition.
DEFINE_SINGLETON(OpenGLRenderingSystem);

/*
*	Pre-initializes the OpenGL rendering system.
*/
void OpenGLRenderingSystem::PreInitializeSystem() NOEXCEPT
{

}

/*
*	Post-initializes the OpenGL rendering system.
*/
void OpenGLRenderingSystem::PostInitializeSystem() NOEXCEPT
{

}

/*
*	Pre-updates the OpenGL rendering system synchronously.
*/
void OpenGLRenderingSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	
}

/*
*	Post-updates the OpenGL rendering system synchronously.
*/
void OpenGLRenderingSystem::PostUpdateSystemSynchronous() NOEXCEPT
{
	
}

/*
*	Releases the OpenGL rendering system.
*/
void OpenGLRenderingSystem::ReleaseSystem() NOEXCEPT
{

}

/*
*	Returns the number of frame buffers
*/
uint8 OpenGLRenderingSystem::GetNumberOfFrameBuffers() const NOEXCEPT
{
	return 1;
}

/*
*	Returns the current frame buffer index.
*/
uint8 OpenGLRenderingSystem::GetCurrentFrameBufferIndex() const NOEXCEPT
{
	return 0;
}

/*
*	Creates a buffer.
*/
ConstantBufferHandle OpenGLRenderingSystem::CreateBuffer(const uint64 size) const NOEXCEPT
{
	return nullptr;
}

/*
*	Uploads data to a buffer.
*/
void OpenGLRenderingSystem::UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT dataSizes, const uint8 dataChunks, ConstantBufferHandle handle) const NOEXCEPT
{
	
}

/*
*	Destroys a constant buffer.
*/
void OpenGLRenderingSystem::DestroyConstantBuffer(ConstantBufferHandle handle) NOEXCEPT
{
	
}

/*
*	Creates a depth buffer.
*/
DepthBufferHandle OpenGLRenderingSystem::CreateDepthBuffer(const Resolution resolution) const NOEXCEPT
{
	return nullptr;
}

/*
*	Destroys a render data table.
*/
void OpenGLRenderingSystem::DestroyRenderDataTable(RenderDataTableHandle handle) NOEXCEPT
{
	
}

/*
*	Creates a render target.
*/
void OpenGLRenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Creates and returns a 2D texture given the texture data.
*/
Texture2DHandle OpenGLRenderingSystem::Create2DTexture(const TextureData &textureData) const NOEXCEPT
{
	return nullptr;
}

/*
*	Destroys a texture 2D
*/
void OpenGLRenderingSystem::DestroyTexture2D(Texture2DHandle handle) NOEXCEPT
{
	
}

/*
*	Creates a texture cube.
*/
TextureCubeHandle OpenGLRenderingSystem::CreateTextureCube(const float *const RESTRICT data, const Resolution resolution) const NOEXCEPT
{
	return nullptr;
}

/*
*	Creates and returns a sampler.
*/
SamplerHandle OpenGLRenderingSystem::CreateSampler(const SamplerProperties &properties) const NOEXCEPT
{
	return nullptr;
}

/*
*	Creates a uniform buffer and returns the identifier for the uniform buffer.
*/
UniformBufferHandle OpenGLRenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize, const BufferUsage usage) const NOEXCEPT
{
	return nullptr;
}

/*
*	Uploads data to a uniform buffer.
*/
void OpenGLRenderingSystem::UploadDataToUniformBuffer(UniformBufferHandle handle, const void *const RESTRICT data) const NOEXCEPT
{
	
}

/*
*	Destroys a uniform buffer.
*/
void OpenGLRenderingSystem::DestroyUniformBuffer(UniformBufferHandle handle) NOEXCEPT
{
	
}

/*
*	Finalizes the initialization of a render pass.
*/
void OpenGLRenderingSystem::FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Add an empty command buffer for now.
	renderPass->SetNumberOfCommandBuffers(1);
	renderPass->AddCommandBuffer(new (MemoryUtilities::GlobalPoolAllocate<sizeof(CommandBuffer)>()) CommandBuffer(nullptr));
}

/*
*	Creates a render data table layout.
*/
void OpenGLRenderingSystem::CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 numberOfBindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Creates a render data table.
*/
void OpenGLRenderingSystem::CreateRenderDataTable(const RenderDataTableLayoutHandle renderDataTableLayout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Binds a combined image sampler to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void OpenGLRenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT
{
	
}

/*
*	Binds a sampled image to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void OpenGLRenderingSystem::BindSampledImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, OpaqueHandle image) const NOEXCEPT
{
	
}

/*
*	Binds a sampler to a render data table.
*/
void OpenGLRenderingSystem::BindSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, SamplerHandle sampler) const NOEXCEPT
{
	
}

/*
*	Binds a uniform buffer to a render data table.
*/
void OpenGLRenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, UniformBufferHandle uniformBuffer) const NOEXCEPT
{
	
}
#endif