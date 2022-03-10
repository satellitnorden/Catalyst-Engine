#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/Map.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Rendering/Native/DebugRenderingSystem.h>
#endif
#include <Rendering/Native/DynamicUniformData.h>
#include <Rendering/Native/GlobalRenderData.h>
#include <Rendering/Native/LightingSystem.h>
#include <Rendering/Native/MaterialSystem.h>
#include <Rendering/Native/ModelSystem.h>
#include <Rendering/Native/PostProcessingSystem.h>
#include <Rendering/Native/RayTracingSystem.h>
#include <Rendering/Native/RenderingConfiguration.h>
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/SamplerProperties.h>
#include <Rendering/Native/SubRenderingSystemInterface.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>
#include <Rendering/Native/VirtualRealitySystem.h>
#include <Rendering/Native/Pipelines/Core/Pipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>
#include <Rendering/Native/RenderingReference/RenderingReferenceSystem.h>

//Resources.
#include <Resources/Core/Texture2DResource.h>

//World.
#include <World/Core/WorldTransform.h>

//Forward declarations.
class CatalystProjectRenderingConfiguration;
class Entity;
class Resolution;
class TextureCubeData;

class ALIGN(8) RenderingSystem final
{

public:

	//System declaration.
	DECLARE_SINGLETON(RenderingSystem);

	/*
	*	Default constructor.
	*/
	RenderingSystem() NOEXCEPT
	{
	
	}

	/*
	*	Initializes the rendering system.
	*/
	void Initialize(const CatalystProjectRenderingConfiguration &configuration) NOEXCEPT;

	/*
	*	Post initializes the rendering system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the rendering system during the render update phase.
	*/
	void RenderUpdate() NOEXCEPT;

	/*
	*	Terminates the rendering system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the current rendering path.
	*/
	FORCE_INLINE NO_DISCARD RenderingPath GetCurrentRenderingPath() const NOEXCEPT
	{
		return _CurrentRenderingPath;
	}

	/*
	*	Sets the current rendering path.
	*/
	void SetCurrentRenderingPath(const RenderingPath value) NOEXCEPT;

	/*
	*	Returns whether or not multiview is supported.
	*/
	NO_DISCARD bool IsMultiviewSupported() const NOEXCEPT;

	/*
	*	Returns whether or not ray tracing is supported.
	*/
	NO_DISCARD bool IsRayTracingSupported() const NOEXCEPT;

	/*
	*	Returns whether or not ray tracing is active.
	*/
	NO_DISCARD bool IsRayTracingActive() const NOEXCEPT;

	/*
	*	Returns whether or not ray tracing is possible.
	*/
	NO_DISCARD bool IsRayTracingPossible() const NOEXCEPT;

	/*
	*	Returns the rendering configuration.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD RenderingConfiguration *const RESTRICT GetRenderingConfiguration() NOEXCEPT
	{
		return &_RenderingConfiguration;
	}

	/*
	*	Returns the resolution.
	*/
	FORCE_INLINE NO_DISCARD Resolution GetFullResolution() const NOEXCEPT
	{
		return _FullResolution;
	}

	/*
	*	Returns the full aspect ratio.
	*/
	FORCE_INLINE NO_DISCARD float32 GetFullAspectRatio() const NOEXCEPT
	{
		return static_cast<float32>(_FullResolution._Width) / static_cast<float32>(_FullResolution._Height);
	}

	/*
	*	Returns the scaled resolution.
	*/
	FORCE_INLINE NO_DISCARD Resolution GetScaledResolution(const uint8 mip_level) const NOEXCEPT
	{
		return _ScaledResolutions[mip_level];
	}

	/*
	*	Returns the scaled aspect ratio.
	*/
	FORCE_INLINE NO_DISCARD float32 GetScaledAspectRatio() const NOEXCEPT
	{
		return static_cast<float32>(_ScaledResolutions[0]._Width) / static_cast<float32>(_ScaledResolutions[0]._Height);
	}
	
	/*
	*	Returns the number of framebuffers.
	*/
	NO_DISCARD uint8 GetNumberOfFramebuffers() const NOEXCEPT;

	/*
	*	Returns the current framebuffer index.
	*/
	NO_DISCARD uint8 GetCurrentFramebufferIndex() const NOEXCEPT;

	/*
	*	Returns the current surface transform.
	*/
	NO_DISCARD SurfaceTransform GetCurrentSurfaceTransform() const NOEXCEPT;

	/*
	*	Returns the current surface transform rotation matrix.
	*/
	Matrix4x4 GetCurrentSurfaceTransformRotationMatrix() const NOEXCEPT;

	/*
	*	Returns the given render target.
	*/
	RenderTargetHandle GetRenderTarget(const RenderTarget render_target) NOEXCEPT;

	/*
	*	Returns a sampler with the given sampler properties.
	*/
	SamplerHandle GetSampler(const Sampler sampler) const NOEXCEPT
	{
		return _Samplers[UNDERLYING(sampler)];
	}
	
#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Returns the debug rendering system.
	*/
	RESTRICTED NO_DISCARD DebugRenderingSystem *const RESTRICT GetDebugRenderingSystem() NOEXCEPT
	{
		return &_DebugRenderingSystem;
	}
#endif

	/*
	*	Returns the lighting system.
	*/
	RESTRICTED NO_DISCARD LightingSystem *const RESTRICT GetLightingSystem() NOEXCEPT
	{
		return &_LightingSystem;
	}

	/*
	*	Returns the material system.
	*/
	RESTRICTED NO_DISCARD MaterialSystem *const RESTRICT GetMaterialSystem() NOEXCEPT
	{
		return &_MaterialSystem;
	}

	/*
	*	Returns the model system.
	*/
	RESTRICTED NO_DISCARD ModelSystem *const RESTRICT GetModelSystem() NOEXCEPT
	{
		return &_ModelSystem;
	}

	/*
	*	Returns the post processing system.
	*/
	RESTRICTED NO_DISCARD PostProcessingSystem *const RESTRICT GetPostProcessingSystem() NOEXCEPT
	{
		return &_PostProcessingSystem;
	}

	/*
	*	Returns the ray tracing system.
	*/
	RESTRICTED NO_DISCARD RayTracingSystem *const RESTRICT GetRayTracingSystem() NOEXCEPT
	{
		return &_RayTracingSystem;
	}

	/*
	*	Returns the rendering reference system.
	*/
	RESTRICTED NO_DISCARD RenderingReferenceSystem *const RESTRICT GetRenderingReferenceSystem() NOEXCEPT
	{
		return &_RenderingReferenceSystem;
	}

	/*
	*	Returns the virtual reality system.
	*/
	RESTRICTED NO_DISCARD VirtualRealitySystem *const RESTRICT GetVirtualRealitySystem() NOEXCEPT
	{
		return &_VirtualRealitySystem;
	}

	/*
	*	Creates a bottom level acceleration structure.
	*/
	void CreateBottomLevelAccelerationStructure(const BufferHandle vertex_buffer,
												const uint32 number_of_vertices,
												const BufferHandle index_buffer,
												const uint32 number_of_indices,
												AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Creates a top level acceleration structure.
	*/
	void CreateTopLevelAccelerationStructure(const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instance_data, AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Destroys an acceleration structure.
	*/
	void DestroyAccelerationStructure(AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Creates a buffer.
	*/
	void CreateBuffer(const uint64 size, const BufferUsage usage, const MemoryProperty memory_properties, BufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Uploads data to a buffer.
	*/
	void UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT data_sizes, const uint8 data_chunks, BufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Destroys a buffer.
	*/
	void DestroyBuffer(BufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a command pool.
	*/
	void CreateCommandPool(const Pipeline::Type type, CommandPoolHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Resets a command pool.
	*/
	void ResetCommandPool(CommandPoolHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Allocates a command buffer from the given command pool.
	*/
	RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT AllocateCommandBuffer(const CommandPoolHandle command_pool, const CommandBufferLevel level) const NOEXCEPT;

	/*
	*	Returns a global command buffer from the global command pool.
	*/
	RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT GetGlobalCommandBuffer(const CommandBufferLevel level) NOEXCEPT;

	/*
	*	Submits a command buffer.
	*/
	void SubmitCommandBuffer(const CommandBuffer *const RESTRICT command_buffer) NOEXCEPT;

	/*
	*	Creates a depth buffer.
	*/
	void CreateDepthBuffer(const Resolution resolution, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Destroys a depth buffer.
	*/
	void DestroyDepthBuffer(DepthBufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates an event.
	*/
	void CreateEvent(EventHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Resets an event.
	*/
	void ResetEvent(const EventHandle handle) NOEXCEPT;

	/*
	*	Waits for an event.
	*/
	void WaitForEvent(const EventHandle handle) NOEXCEPT;

	/*
	*	Creates a query pool.
	*/
	void CreateQueryPool(QueryPoolHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Returns the execution time, in nanoseconds, from the given query pool.
	*	Assumption being that the query pool has been used to record two timestamps into a command buffer that has completed.
	*/
	NO_DISCARD uint32 GetExecutionTime(const QueryPoolHandle query_pool) NOEXCEPT;

	/*
	*	Creates a Shader.
	*/
	void CreateShader(const ArrayProxy<byte> &data, const ShaderStage stage, ShaderHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a render data table layout.
	*/
	void CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 number_of_bindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Destroys a render data table layout.
	*/
	void DestroyRenderDataTableLayout(RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a render data table.
	*/
	void CreateRenderDataTable(const RenderDataTableLayoutHandle render_data_table_layout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Binds an acceleration structure to a render data table.
	*/
	void BindAccelerationStructureToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const AccelerationStructureHandle acceleration_structure) const NOEXCEPT;

	/*
	*	Binds a combined image sampler to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	void BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image, const SamplerHandle sampler) const NOEXCEPT;

	/*
	*	Binds a sampled image to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	void BindSampledImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT;

	/*
	*	Binds a sampler to a render data table.
	*/
	void BindSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const SamplerHandle sampler) const NOEXCEPT;

	/*
	*	Binds a storage buffer to a render data table.
	*/
	void BindStorageBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT;

	/*
	*	Binds a storage image to a render data table.
	*/
	void BindStorageImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT;

	/*
	*	Binds a uniform buffer to a render data table.
	*/
	void BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT;

	/*
	*	Destroys a render data table.
	*/
	void DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a render target.
	*/
	void CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Destroys a render target.
	*/
	void DestroyRenderTarget(RenderTargetHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates and returns a sampler.
	*/
	void CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a texture 2D.
	*/
	void CreateTexture2D(const TextureData &data, Texture2DHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Destroys a texture 2D.
	*/
	void DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a texture 3D.
	*/
	void CreateTexture3D(const TextureData &data, Texture3DHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a texture cube.
	*/
	void CreateTextureCube(const TextureCubeData &data, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Initializes a render pass.
	*/
	void InitializeRenderPass(RenderPass *const RESTRICT render_pass) NOEXCEPT;

	/*
	*	Initializes a pipeline
	*/
	void InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT;

	/*
	*	Terminates a pipeline.
	*/
	void TerminatePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT;

	/*
	*	Terminates a render pass.
	*/
	void TerminateRenderPass(RenderPass *const RESTRICT render_pass) const NOEXCEPT;

	/*
	*	Returns the global render data table.
	*/
	RenderDataTableHandle GetGlobalRenderDataTable() const NOEXCEPT;

	/*
	*	Adds a texture to the global render data and returns it's index.
	*/
	uint32 AddTextureToGlobalRenderData(Texture2DHandle texture) NOEXCEPT;

	/*
	*	Returns a texture to the global render data and marks it's index as available.
	*/
	void ReturnTextureToGlobalRenderData(const uint32 index) NOEXCEPT;

	/*
	*	Returns the given common render data table layout.
	*/
	RenderDataTableLayoutHandle GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) const NOEXCEPT;

	/*
	*	Returns the given common render data table.
	*/
	RenderDataTableHandle GetCommonRenderDataTable(const CommonRenderDataTable common_render_data_table) const NOEXCEPT;

	/*
	*	Returns the render passes.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<RenderPass *RESTRICT> &GetRenderPasses() const NOEXCEPT
	{
		return _RenderPasses;
	}

	/*
	*	Takes an immedate screenshot and writes it to the given file path.
	*/
	void TakeImmediateScreenshot(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Starts taking a screenshot.
	*	This will pause the game, turn off denoising for all effects and start accumulating results across frames.
	*	This can be used to, for example, take reference screenshots to tweak visual effects, or to take "prettier" screenshots of the game.
	*/
	void StartTakingScreenshot() NOEXCEPT;

	/*
	*	Returns if the rendering system is currently taking a screenshot.
	*/
	NO_DISCARD bool IsTakingScreenshot() NOEXCEPT;

	/*
	*	Stops taking a screenshot and writes the result to the given file path.
	*/
	void StopTakingScreenshot(const char *const RESTRICT file_path) NOEXCEPT;

private:

	//The rendering configuration.
	RenderingConfiguration _RenderingConfiguration;

	//The sub rendering system.
	SubRenderingSystemInterface *RESTRICT _SubRenderingSystem;

	//The current rendering path.
	RenderingPath _CurrentRenderingPath;

	//The full resolution.
	Resolution _FullResolution;

	//The scaled resolutions.
	StaticArray<Resolution, 10> _ScaledResolutions;

	//The global render data.
	GlobalRenderData _GlobalRenderData;

	//Container for all render targets.
	StaticArray<RenderTargetHandle, UNDERLYING(RenderTarget::NUMBER_OF_RENDER_TARGETS)> _RenderTargets;

	//Container for all samplers.
	StaticArray<SamplerHandle, UNDERLYING(Sampler::NumberOfSamplers)> _Samplers;

	//Container for all common render data table layouts.
	StaticArray<RenderDataTableLayoutHandle, UNDERLYING(CommonRenderDataTableLayout::NUMBER_OF_COMMON_RENDER_DATA_TABLE_LAYOUTS)> _CommonRenderDataTableLayouts;

	//Container for all common render data tables.
	StaticArray<RenderDataTableHandle, UNDERLYING(CommonRenderDataTable::NUMBER_OF_COMMON_RENDER_DATA_TABLES)> _CommonRenderDataTables;

	//The default texture 2D.
	Texture2DHandle _DefaultTexture2D;

	//The dynamic uniform data.
	DynamicUniformData _DynamicUniformData;

	//The current jitter index.
	uint8 _CurrentJitterIndex{ 0 };

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The debug rendering system.
	DebugRenderingSystem _DebugRenderingSystem;
#endif

	//The lighting system.
	LightingSystem _LightingSystem;

	//The material system.
	MaterialSystem _MaterialSystem;

	//The model system.
	ModelSystem _ModelSystem;

	//The post processing system.
	PostProcessingSystem _PostProcessingSystem;

	//The ray tracing system.
	RayTracingSystem _RayTracingSystem;

	//The rendering reference system.
	RenderingReferenceSystem _RenderingReferenceSystem;

	//The virtual reality system.
	VirtualRealitySystem _VirtualRealitySystem;

	//Container for all the render passes.
	DynamicArray<RenderPass *RESTRICT> _RenderPasses;

	//The current blue noise texture index.
	uint8 _CurrentBlueNoiseTextureIndex{ 0 };

	//The Hammersley hemisphere samples uniform buffer.
	BufferHandle _HammersleyHemisphereSamplesUniformBuffer;

	//The previous Perceiver world transform.
	WorldTransform _PreviousPerceiverWorldTransform;

	//The current Perceiver world transform
	WorldTransform _CurrentPerceiverWorldTransform;

	//Denotes if the rendering system is currently taking a screenshot.
	bool _IsTakingScreenshot{ false };

	/*
	*	Pre-initializes the global render data.
	*/
	void PreInitializeGlobalRenderData() NOEXCEPT;

	/*
	*	Initializes all render targets.
	*/
	void InitializeRenderTargets() NOEXCEPT;

	/*
	*	Initializes all samplers.
	*/
	void InitializeSamplers() NOEXCEPT;

	/*
	*	Initializes all common render data table layouts.
	*/
	void InitializeCommonRenderDataTableLayouts() NOEXCEPT;

	/*
	*	Initializes all common render data tables.
	*/
	void InitializeCommonRenderDataTables() NOEXCEPT;

	/*
	*	Initializes the default texture.
	*/
	void InitializeDefaultTexture() NOEXCEPT;

	/*
	*	Initializes the Hammersley hemisphere samples uniform buffer.
	*/
	void InitializeHammersleyHemisphereSamplesUniformBuffer() NOEXCEPT;

	/*
	*	Post-initializes the global render data.
	*/
	void PostInitializeGlobalRenderData() NOEXCEPT;

	/*
	*	Post-initializes the common render data tables.
	*/
	void PostInitializeCommonRenderDataTables() NOEXCEPT;

	/*
	*	Updates the global render data.
	*/
	void UpdateGlobalRenderData() NOEXCEPT;

	/*
	*	Updates the global uniform data
	*/
	void UpdateGlobalUniformData(const uint8 current_framebuffer_index) NOEXCEPT;

	/*
	*	Updates the global textures.
	*/
	void UpdateGlobalTextures(const uint8 current_framebuffer_index) NOEXCEPT;

	/*
	*	Updates the global materials.
	*/
	void UpdateGlobalMaterials(const uint8 current_framebuffer_index) NOEXCEPT;

	/*
	*	Updates the global command pool data.
	*/
	void UpdateGlobalCommandPoolData(const uint8 current_framebuffer_index) NOEXCEPT;

	/*
	*	Updates the terrain height textures.
	*/
	void UpdateTerrainHeightTextures(const uint8 currentFrameBufferIndex) NOEXCEPT;

	/*
	*	Updates the terrain patch data.
	*/
	void UpdateTerrainPatchData(const uint8 currentFrameBufferIndex) NOEXCEPT;

	/*
	*	Updates the terrain material data.
	*/
	void UpdateTerrainMaterialData(const uint8 currentFrameBufferIndex) NOEXCEPT;

	/*
	*	Initializes all common materials.
	*/
	void InitializeCommonMaterials() NOEXCEPT;

};