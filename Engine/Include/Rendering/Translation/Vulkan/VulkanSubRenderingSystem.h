#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/SubRenderingSystemInterface.h>

class VulkanSubRenderingSystem final : public SubRenderingSystemInterface
{

public:

	/*
	*	Pre-initializes the sub rendering system.
	*/
	void PreInitialize() NOEXCEPT override;

	/*
	*	Initializes the sub rendering system.
	*/
	void Initialize() NOEXCEPT override;

	/*
	*	Post-initializes the sub rendering system.
	*/
	void PostInitialize() NOEXCEPT override;

	/*
	*	Terminates the sub rendering system.
	*/
	void Terminate() NOEXCEPT override;

	/*
	*	Returns whether or not this sub rendering system can render.
	*/
	NO_DISCARD bool CanRender() const NOEXCEPT override;

	/*
	*	Notifies this sub rendering system that a rendering platform event has occured.
	*/
	void OnRenderingPlatformEvent(const RenderingPlatformEvent rendering_platform_event) NOEXCEPT override;

	/*
	*	Returns whether or not geometry shaders is supported.
	*/
	NO_DISCARD bool IsGeometryShadersSupported() const NOEXCEPT override;

	/*
	*	Returns whether or not multiview is supported.
	*/
	NO_DISCARD bool IsMultiviewSupported() const NOEXCEPT override;

	/*
	*	Returns whether or not ray tracing is supported.
	*/
	NO_DISCARD bool IsRayTracingSupported() const NOEXCEPT override;

	/*
	*	Returns the number of framebuffers.
	*/
	NO_DISCARD uint8 GetNumberOfFramebuffers() const NOEXCEPT override;

	/*
	*	Returns the current framebuffer index.
	*/
	NO_DISCARD uint8 GetCurrentFramebufferIndex() const NOEXCEPT override;

	/*
	*	Returns the current surface transform.
	*/
	NO_DISCARD SurfaceTransform GetCurrentSurfaceTransform() const NOEXCEPT override;

	/*
	*	Creates a bottom level acceleration structure.
	*/
	void CreateBottomLevelAccelerationStructure(const BufferHandle vertex_buffer,
												const uint32 number_of_vertices,
												const BufferHandle index_buffer,
												const uint32 number_of_indices,
												const BottomLevelAccelerationStructureFlag flags,
												AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT override;

	/*
	*	Creates a top level acceleration structure.
	*	Can take an optional command buffer if one is created during a frame.
	*/
	void CreateTopLevelAccelerationStructure
	(
		const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instance_data,
		AccelerationStructureHandle *const RESTRICT handle,
		CommandBuffer *const RESTRICT command_buffer
	) NOEXCEPT override;

	/*
	*	Destroys an acceleration structure.
	*/
	void DestroyAccelerationStructure(AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT override;

	/*
	*	Creates a buffer.
	*/
	void CreateBuffer(const uint64 size, const BufferUsage usage, const MemoryProperty memory_properties, BufferHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Uploads data to a buffer.
	*/
	void UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT data_sizes, const uint8 data_chunks, BufferHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Destroys a buffer.
	*/
	void DestroyBuffer(BufferHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates a command pool.
	*/
	void CreateCommandPool(const Pipeline::Type type, CommandPoolHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Resets a command pool.
	*/
	void ResetCommandPool(CommandPoolHandle *const RESTRICT handle) NOEXCEPT override;

	/*
	*	Allocates a command buffer from the given command pool.
	*/
	RESTRICTED NO_DISCARD CommandBuffer* const RESTRICT AllocateCommandBuffer(const CommandPoolHandle command_pool, const CommandBufferLevel level) const NOEXCEPT override;

	/*
	*	Submits a command buffer.
	*/
	void SubmitCommandBuffer(const CommandBuffer *const RESTRICT command_buffer) NOEXCEPT override;

	/*
	*	Creates a depth buffer.
	*/
	void CreateDepthBuffer(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Destroys a depth buffer.
	*/
	void DestroyDepthBuffer(DepthBufferHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates an event.
	*/
	void CreateEvent(EventHandle *const RESTRICT handle) NOEXCEPT override;

	/*
	*	Resets an event.
	*/
	void ResetEvent(const EventHandle handle) NOEXCEPT override;

	/*
	*	Waits for an event.
	*/
	void WaitForEvent(const EventHandle handle) NOEXCEPT override;

	/*
	*	Creates a query pool.
	*/
	void CreateQueryPool(const uint32 query_count, QueryPoolHandle *const RESTRICT handle) NOEXCEPT override;

	/*
	*	Resets the given query pool.
	*/
	void ResetQueryPool(QueryPoolHandle *const RESTRICT handle) NOEXCEPT override;

	/*
	*	Returns the execution time, in nanoseconds, from the given query pool.
	*	Assumption being that the query pool has been used to record two timestamps into a command buffer that has completed.
	*/
	NO_DISCARD uint64 GetExecutionTime(const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT override;

	/*
	*	Creates a Shader.
	*/
	void CreateShader(const ArrayProxy<byte> &data, const ShaderStage stage, ShaderHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates a render data table layout.
	*/
	void CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 number_of_bindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Destroys a render data table layout.
	*/
	void DestroyRenderDataTableLayout(RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates a render data table.
	*/
	void CreateRenderDataTable(const RenderDataTableLayoutHandle render_data_table_layout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Binds an acceleration structure to a render data table.
	*/
	void BindAccelerationStructureToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const AccelerationStructureHandle acceleration_structure) const NOEXCEPT override;

	/*
	*	Binds a combined image sampler to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	void BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image, const SamplerHandle sampler) const NOEXCEPT override;

	/*
	*	Binds a sampled image to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	void BindSampledImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT override;

	/*
	*	Binds a sampler to a render data table.
	*/
	void BindSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const SamplerHandle sampler) const NOEXCEPT override;

	/*
	*	Binds a storage buffer to a render data table.
	*/
	void BindStorageBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT override;

	/*
	*	Binds a storage image to a render data table.
	*/
	void BindStorageImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT override;

	/*
	*	Binds a uniform buffer to a render data table.
	*/
	void BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT override;

	/*
	*	Destroys a render data table.
	*/
	void DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates a render target.
	*/
	void CreateRenderTarget(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Destroys a render target.
	*/
	void DestroyRenderTarget(RenderTargetHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates a sampler.
	*/
	void CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates a texture 2D.
	*/
	void CreateTexture2D(const TextureData &data, Texture2DHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Updates a texture 2D.
	*/
	void UpdateTexture2D(const TextureDataContainer &texture_data_container, Texture2DHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Destroys a texture 2D.
	*/
	void DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates a texture 3D.
	*/
	void CreateTexture3D(const TextureData &data, Texture3DHandle *const RESTRICT handle) const NOEXCEPT override;

	/*
	*	Creates a texture cube.
	*/
	void CreateTextureCube(const TextureCubeData &data, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT override;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Creates an ImGui texture.
	*/
	NO_DISCARD ImTextureID CreateImGuiTexture(const Texture2DHandle texture) NOEXCEPT override;
#endif

	/*
	*	Initializes a pipeline
	*/
	void InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT override;

	/*
	*	Terminates a pipeline
	*/
	void TerminatePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT override;

	/*
	*	Takes an immedate screenshot and writes it to the given file path.
	*/
	void TakeImmediateScreenshot(const char *const RESTRICT file_path) NOEXCEPT override;

	/*
	*	Begins a frame.
	*/
	void BeginFrame() NOEXCEPT override;

	/*
	*	Ends a frame.
	*/
	void EndFrame(const CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT override;

};