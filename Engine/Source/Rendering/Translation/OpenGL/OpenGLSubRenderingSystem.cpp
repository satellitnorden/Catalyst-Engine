#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Translation/OpenGL/OpenGLSubRenderingSystem.h>

/*
*	Pre-initializes the sub rendering system.
*/
void OpenGLSubRenderingSystem::PreInitialize() NOEXCEPT
{

}

/*
*	Initializes the sub rendering system.
*/
void OpenGLSubRenderingSystem::Initialize() NOEXCEPT
{

}

/*
*	Post-initializes the sub rendering system.
*/
void OpenGLSubRenderingSystem::PostInitialize() NOEXCEPT
{

}

/*
*	Terminates the sub rendering system.
*/
void OpenGLSubRenderingSystem::Terminate() NOEXCEPT
{
	
}

/*
*	Returns whether or not geometry shaders is supported.
*/
NO_DISCARD bool OpenGLSubRenderingSystem::IsGeometryShadersSupported() const NOEXCEPT
{
	return false;
}

/*
*	Returns whether or not multiview is supported.
*/
NO_DISCARD bool OpenGLSubRenderingSystem::IsMultiviewSupported() const NOEXCEPT
{
	return false;
}

/*
*	Returns whether or not ray tracing is supported.
*/
NO_DISCARD bool OpenGLSubRenderingSystem::IsRayTracingSupported() const NOEXCEPT
{
	return false;
}

/*
*	Returns the number of framebuffers.
*/
uint8 OpenGLSubRenderingSystem::GetNumberOfFramebuffers() const NOEXCEPT
{
	return 0;
}

/*
*	Returns the current framebuffer index.
*/
uint8 OpenGLSubRenderingSystem::GetCurrentFramebufferIndex() const NOEXCEPT
{
	return 0;
}

/*
*	Returns the current surface transform.
*/
SurfaceTransform OpenGLSubRenderingSystem::GetCurrentSurfaceTransform() const NOEXCEPT
{
	return SurfaceTransform::IDENTITY;
}

/*
*	Creates a bottom level acceleration structure.
*/
void OpenGLSubRenderingSystem::CreateBottomLevelAccelerationStructure(	const BufferHandle vertex_buffer,
																		const uint32 number_of_vertices,
																		const BufferHandle index_buffer,
																		const uint32 number_of_indices,
																		const BottomLevelAccelerationStructureFlag flags,
																		AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{
	
}

/*
*	Creates a top level acceleration structure.
*	Can take an optional command buffer if one is created during a frame.
*/
void OpenGLSubRenderingSystem::CreateTopLevelAccelerationStructure
(
	const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instance_data,
	AccelerationStructureHandle *const RESTRICT handle,
	CommandBuffer *const RESTRICT command_buffer
) NOEXCEPT
{

}

/*
*	Destroys an acceleration structure.
*/
void OpenGLSubRenderingSystem::DestroyAccelerationStructure(AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{

}

/*
*	Creates a buffer.
*/
void OpenGLSubRenderingSystem::CreateBuffer(const uint64 size, const BufferUsage usage, const MemoryProperty memory_properties, BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Uploads data to a buffer.
*/
void OpenGLSubRenderingSystem::UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT data_sizes, const uint8 data_chunks, BufferHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Destroys a buffer.
*/
void OpenGLSubRenderingSystem::DestroyBuffer(BufferHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Creates a command pool.
*/
void OpenGLSubRenderingSystem::CreateCommandPool(const Pipeline::Type type, CommandPoolHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Resets a command pool.
*/
void OpenGLSubRenderingSystem::ResetCommandPool(CommandPoolHandle *const RESTRICT handle) NOEXCEPT
{

}

/*
*	Allocates a command buffer from the given command pool.
*/
RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT OpenGLSubRenderingSystem::AllocateCommandBuffer(const CommandPoolHandle command_pool, const CommandBufferLevel level) const NOEXCEPT
{
	return nullptr;
}

/*
*	Submits a command buffer.
*/
void OpenGLSubRenderingSystem::SubmitCommandBuffer(const CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{

}

/*
*	Creates a depth buffer.
*/
void OpenGLSubRenderingSystem::CreateDepthBuffer(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Destroys a depth buffer.
*/
void OpenGLSubRenderingSystem::DestroyDepthBuffer(DepthBufferHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Creates an event.
*/
void OpenGLSubRenderingSystem::CreateEvent(EventHandle *const RESTRICT handle) NOEXCEPT
{

}

/*
*	Resets an event.
*/
void OpenGLSubRenderingSystem::ResetEvent(EventHandle handle) NOEXCEPT
{

}

/*
*	Waits for an event.
*/
void OpenGLSubRenderingSystem::WaitForEvent(EventHandle handle) NOEXCEPT
{

}

/*
*	Creates a query pool.
*/
void OpenGLSubRenderingSystem::CreateQueryPool(const uint32 query_count, QueryPoolHandle *const RESTRICT handle) NOEXCEPT
{

}

/*
*	Resets the given query pool.
*/
void OpenGLSubRenderingSystem::ResetQueryPool(QueryPoolHandle *const RESTRICT handle) NOEXCEPT
{

}

/*
*	Returns the execution time, in nanoseconds, from the given query pool.
*	Assumption being that the query pool has been used to record two timestamps into a command buffer that has completed.
*/
NO_DISCARD uint64 OpenGLSubRenderingSystem::GetExecutionTime(const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT
{
	return 0;
}

/*
*	Creates a Shader.
*/
void OpenGLSubRenderingSystem::CreateShader(const ArrayProxy<byte> &data, const ShaderStage stage, ShaderHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Creates a render data table layout.
*/
void OpenGLSubRenderingSystem::CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 number_of_bindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Destroys a render data table layout.
*/
void OpenGLSubRenderingSystem::DestroyRenderDataTableLayout(RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Creates a render data table.
*/
void OpenGLSubRenderingSystem::CreateRenderDataTable(const RenderDataTableLayoutHandle render_data_table_layout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Binds an acceleration structure to a render data table.
*/
void OpenGLSubRenderingSystem::BindAccelerationStructureToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const AccelerationStructureHandle acceleration_structure) const NOEXCEPT
{
	
}

/*
*	Binds a combined image sampler to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void OpenGLSubRenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image, const SamplerHandle sampler) const NOEXCEPT
{
	
}

/*
*	Binds a sampled image to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void OpenGLSubRenderingSystem::BindSampledImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT
{
	
}

/*
*	Binds a sampler to a render data table.
*/
void OpenGLSubRenderingSystem::BindSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const SamplerHandle sampler) const NOEXCEPT
{
	
}

/*
*	Binds a storage image to a render data table.
*/
void OpenGLSubRenderingSystem::BindStorageImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT
{
	
}

/*
*	Binds a storage buffer to a render data table.
*/
void OpenGLSubRenderingSystem::BindStorageBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT
{
	
}

/*
*	Binds a uniform buffer to a render data table.
*/
void OpenGLSubRenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT
{
	
}

/*
*	Destroys a render data table.
*/
void OpenGLSubRenderingSystem::DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Creates a render target.
*/
void OpenGLSubRenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Destroys a render target.
*/
void OpenGLSubRenderingSystem::DestroyRenderTarget(RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Creates a sampler.
*/
void OpenGLSubRenderingSystem::CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Creates a texture 2D.
*/
void OpenGLSubRenderingSystem::CreateTexture2D(const TextureData &data, Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Updates a texture 2D.
*/
void OpenGLSubRenderingSystem::UpdateTexture2D(const TextureDataContainer &texture_data_container, Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Destroys a texture 2D.
*/
void OpenGLSubRenderingSystem::DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{

}

/*
*	Creates a texture 3D.
*/
void OpenGLSubRenderingSystem::CreateTexture3D(const TextureData &data, Texture3DHandle *const RESTRICT handle) const NOEXCEPT
{
	
}

/*
*	Creates a texture cube.
*/
void OpenGLSubRenderingSystem::CreateTextureCube(const uint32 resolution, const DynamicArray<DynamicArray<float32>> &data, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT
{

}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Creates an ImGui texture.
*/
NO_DISCARD ImTextureID OpenGLSubRenderingSystem::CreateImGuiTexture(const Texture2DHandle texture) NOEXCEPT
{
	return ImTextureID();
}
#endif

/*
*	Initializes a pipeline
*/
void OpenGLSubRenderingSystem::InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT
{
	
}

/*
*	Terminates a pipeline
*/
void OpenGLSubRenderingSystem::TerminatePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT
{

}

/*
*	Takes an immedate screenshot and writes it to the given file path.
*/
void OpenGLSubRenderingSystem::TakeImmediateScreenshot(const char *const RESTRICT file_path) NOEXCEPT
{
	
}

/*
*	Begins a frame.
*/
void OpenGLSubRenderingSystem::BeginFrame() NOEXCEPT
{
	
}

/*
*	Ends a frame.
*/
void OpenGLSubRenderingSystem::EndFrame(const CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT
{
	
}
#endif