#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/SamplerProperties.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

//Resources.
#include <Resources/Loading/TextureCubeData.h>

class SubRenderingSystemInterface
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~SubRenderingSystemInterface() NOEXCEPT
	{

	}

	/*
	*	Pre-initializes the sub rendering system.
	*/
	virtual void PreInitialize() NOEXCEPT = 0;

	/*
	*	Initializes the sub rendering system.
	*/
	virtual void Initialize() NOEXCEPT = 0;

	/*
	*	Post-initializes the sub rendering system.
	*/
	virtual void PostInitialize() NOEXCEPT = 0;

	/*
	*	Terminates the sub rendering system.
	*/
	virtual void Terminate() NOEXCEPT = 0;

	/*
	*	Returns whether or not this sub rendering system can render.
	*/
	FORCE_INLINE virtual NO_DISCARD bool CanRender() const NOEXCEPT
	{
		return true;
	}

	/*
	*	Notifies this sub rendering system that a rendering platform event has occured.
	*/
	FORCE_INLINE virtual void OnRenderingPlatformEvent(const RenderingPlatformEvent rendering_platform_event) NOEXCEPT
	{

	}

	/*
	*	Returns whether or not geometry shaders is supported.
	*/
	virtual NO_DISCARD bool IsGeometryShadersSupported() const NOEXCEPT = 0;

	/*
	*	Returns whether or not multiview is supported.
	*/
	virtual NO_DISCARD bool IsMultiviewSupported() const NOEXCEPT = 0;

	/*
	*	Returns whether or not ray tracing is supported.
	*/
	virtual NO_DISCARD bool IsRayTracingSupported() const NOEXCEPT = 0;

	/*
	*	Returns the number of framebuffers.
	*/
	virtual NO_DISCARD uint8 GetNumberOfFramebuffers() const NOEXCEPT = 0;

	/*
	*	Returns the current framebuffer index.
	*/
	virtual NO_DISCARD uint8 GetCurrentFramebufferIndex() const NOEXCEPT = 0;

	/*
	*	Returns the current surface transform.
	*/
	virtual NO_DISCARD SurfaceTransform GetCurrentSurfaceTransform() const NOEXCEPT = 0;

	/*
	*	Creates a bottom level acceleration structure.
	*/
	virtual void CreateBottomLevelAccelerationStructure(const BufferHandle vertex_buffer,
														const uint32 number_of_vertices,
														const BufferHandle index_buffer,
														const uint32 number_of_indices,
														AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT = 0;

	/*
	*	Creates a top level acceleration structure.
	*/
	virtual void CreateTopLevelAccelerationStructure(const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instance_data, AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT = 0;

	/*
	*	Destroys an acceleration structure.
	*/
	virtual void DestroyAccelerationStructure(AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT = 0;

	/*
	*	Creates a buffer.
	*/
	virtual void CreateBuffer(const uint64 size, const BufferUsage usage, const MemoryProperty memory_properties, BufferHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Uploads data to a buffer.
	*/
	virtual void UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT data_sizes, const uint8 data_chunks, BufferHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Destroys a buffer.
	*/
	virtual void DestroyBuffer(BufferHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates a command pool.
	*/
	virtual void CreateCommandPool(const Pipeline::Type type, CommandPoolHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Resets a command pool.
	*/
	virtual void ResetCommandPool(CommandPoolHandle *const RESTRICT handle) NOEXCEPT = 0;

	/*
	*	Allocates a command buffer from the given command pool.
	*/
	virtual RESTRICTED NO_DISCARD CommandBuffer* const RESTRICT AllocateCommandBuffer(const CommandPoolHandle command_pool, const CommandBufferLevel level) const NOEXCEPT = 0;

	/*
	*	Submits a command buffer.
	*/
	virtual void SubmitCommandBuffer(const CommandBuffer *const RESTRICT command_buffer) NOEXCEPT = 0;

	/*
	*	Creates a depth buffer.
	*/
	virtual void CreateDepthBuffer(const Resolution resolution, const SampleCount sample_count, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Destroys a depth buffer.
	*/
	virtual void DestroyDepthBuffer(DepthBufferHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates an event.
	*/
	virtual void CreateEvent(EventHandle *const RESTRICT handle) NOEXCEPT = 0;

	/*
	*	Resets an event.
	*/
	virtual void ResetEvent(const EventHandle handle) NOEXCEPT = 0;

	/*
	*	Waits for an event.
	*/
	virtual void WaitForEvent(const EventHandle handle) NOEXCEPT = 0;

	/*
	*	Creates a query pool.
	*/
	virtual void CreateQueryPool(const uint32 query_count, QueryPoolHandle *const RESTRICT handle) NOEXCEPT = 0;

	/*
	*	Resets the given query pool.
	*/
	virtual void ResetQueryPool(QueryPoolHandle *const RESTRICT handle) NOEXCEPT = 0;

	/*
	*	Returns the execution time, in nanoseconds, from the given query pool.
	*	Assumption being that the query pool has been used to record two timestamps into a command buffer that has completed.
	*/
	virtual NO_DISCARD uint64 GetExecutionTime(const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT = 0;

	/*
	*	Creates a Shader.
	*/
	virtual void CreateShader(const ArrayProxy<byte> &data, const ShaderStage stage, ShaderHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates a render data table layout.
	*/
	virtual void CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 number_of_bindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Destroys a render data table layout.
	*/
	virtual void DestroyRenderDataTableLayout(RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates a render data table.
	*/
	virtual void CreateRenderDataTable(const RenderDataTableLayoutHandle render_data_table_layout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Binds an acceleration structure to a render data table.
	*/
	virtual void BindAccelerationStructureToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const AccelerationStructureHandle acceleration_structure) const NOEXCEPT = 0;

	/*
	*	Binds a combined image sampler to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	virtual void BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image, const SamplerHandle sampler) const NOEXCEPT = 0;

	/*
	*	Binds a sampled image to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	virtual void BindSampledImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT = 0;

	/*
	*	Binds a sampler to a render data table.
	*/
	virtual void BindSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const SamplerHandle sampler) const NOEXCEPT = 0;

	/*
	*	Binds a storage buffer to a render data table.
	*/
	virtual void BindStorageBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT = 0;

	/*
	*	Binds a storage image to a render data table.
	*/
	virtual void BindStorageImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT = 0;

	/*
	*	Binds a uniform buffer to a render data table.
	*/
	virtual void BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT = 0;

	/*
	*	Destroys a render data table.
	*/
	virtual void DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates a render target.
	*/
	virtual void CreateRenderTarget(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Destroys a render target.
	*/
	virtual void DestroyRenderTarget(RenderTargetHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates a sampler.
	*/
	virtual void CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates a texture 2D.
	*/
	virtual void CreateTexture2D(const TextureData &data, Texture2DHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Updates a texture 2D.
	*/
	virtual void UpdateTexture2D(const TextureDataContainer &texture_data_container, Texture2DHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Destroys a texture 2D.
	*/
	virtual void DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates a texture 3D.
	*/
	virtual void CreateTexture3D(const TextureData &data, Texture3DHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Creates a texture cube.
	*/
	virtual void CreateTextureCube(const TextureCubeData &data, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT = 0;

	/*
	*	Initializes a pipeline
	*/
	virtual void InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT = 0;

	/*
	*	Terminates a pipeline
	*/
	virtual void TerminatePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT = 0;

	/*
	*	Takes an immedate screenshot and writes it to the given file path.
	*/
	virtual void TakeImmediateScreenshot(const char *const RESTRICT file_path) NOEXCEPT = 0;

	/*
	*	Begins a frame.
	*/
	virtual void BeginFrame() NOEXCEPT = 0;

	/*
	*	Ends a frame.
	*/
	virtual void EndFrame(const CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT = 0;

};