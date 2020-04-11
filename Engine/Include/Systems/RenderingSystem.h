#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/Map.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/DynamicUniformData.h>
#include <Rendering/Native/GlobalRenderData.h>
#include <Rendering/Native/LightingSystem.h>
#include <Rendering/Native/Material.h>
#include <Rendering/Native/MaterialSystem.h>
#include <Rendering/Native/ModelSystem.h>
#include <Rendering/Native/PostProcessingSystem.h>
#include <Rendering/Native/RayTracingSystem.h>
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>
#include <Rendering/Native/Pipelines/Core/Pipeline.h>
#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
#include <Rendering/Native/RenderingReference/RenderingReferenceSystem.h>
#endif

//Resources.
#include <Resources/Core/Texture2DResource.h>

//Forward declarations.
class AxisAlignedBoundingBox;
class CatalystProjectRenderingConfiguration;
class Entity;
class Resolution;
class TextureCubeData;

class RenderingSystem final
{

public:

	//System declaration.
	DECLARE_SINGLETON(RenderingSystem);

	//Enumeration covering all common physical models.
	enum class CommonPhysicalModel : uint8
	{
		Cube,
		Octahedron,
		Plane,
		Sphere,

		NumberOfCommonPhysicalModels
	};

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
	void PostInitializeSystem() NOEXCEPT;

	/*
	*	Updates the rendering system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Releases the rendering system.
	*/
	void Release() NOEXCEPT;

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
	*	Returns the resolution.
	*/
	FORCE_INLINE NO_DISCARD Resolution GetFullResolution(const uint8 mip_level = 0) const NOEXCEPT
	{
		return _FullResolution;
	}

	/*
	*	Returns the scaled resolution.
	*/
	FORCE_INLINE NO_DISCARD Resolution GetScaledResolution(const uint8 mip_level) const NOEXCEPT
	{
		return _ScaledResolutions[mip_level];
	}
	 
	/*
	*	Returns the number of framebuffers.
	*/
	uint8 GetNumberOfFramebuffers() const NOEXCEPT;

	/*
	*	Returns the current framebuffer index.
	*/
	uint8 GetCurrentFramebufferIndex() const NOEXCEPT;

	/*
	*	Returns the given render target.
	*/
	RenderTargetHandle GetRenderTarget(const RenderTarget renderTarget) const NOEXCEPT
	{
		return _RenderTargets[UNDERLYING(renderTarget)];
	}

	/*
	*	Returns a sampler with the given sampler properties.
	*/
	SamplerHandle GetSampler(const Sampler sampler) const NOEXCEPT
	{
		return _Samplers[UNDERLYING(sampler)];
	}
	
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

#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
	/*
	*	Returns the rendering reference system.
	*/
	RESTRICTED NO_DISCARD RenderingReferenceSystem *const RESTRICT GetRenderingReferenceSystem() NOEXCEPT
	{
		return &_RenderingReferenceSystem;
	}
#endif

	/*
	*	Creates a bottom level acceleration structure.
	*/
	void CreateBottomLevelAccelerationStructure(const BufferHandle &vertexBuffer,
												const uint32 numberOfVertices,
												const BufferHandle &indexBuffer,
												const uint32 numberOfIndices,
												AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Creates a top level acceleration structure.
	*/
	void CreateTopLevelAccelerationStructure(const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instanceData, AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Destroys an acceleration structure.
	*/
	void DestroyAccelerationStructure(AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Creates a buffer.
	*/
	void CreateBuffer(const uint64 size, const BufferUsage usage, const MemoryProperty memoryProperties, BufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Uploads data to a buffer.
	*/
	void UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT dataSizes, const uint8 dataChunks, BufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Destroys a buffer.
	*/
	void DestroyBuffer(BufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a depth buffer.
	*/
	void CreateDepthBuffer(const Resolution resolution, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a render data table layout.
	*/
	void CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 numberOfBindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a render data table.
	*/
	void CreateRenderDataTable(const RenderDataTableLayoutHandle renderDataTableLayout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Binds an acceleration structure to a render data table.
	*/
	void BindAccelerationStructureToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, AccelerationStructureHandle accelerationStructure) const NOEXCEPT;

	/*
	*	Binds a combined image sampler to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	void BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT;

	/*
	*	Binds a sampled image to a render data table.
	*	Accepts render target, texture 2D and texture cube handles.
	*/
	void BindSampledImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, OpaqueHandle image) const NOEXCEPT;

	/*
	*	Binds a sampler to a render data table.
	*/
	void BindSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, SamplerHandle sampler) const NOEXCEPT;

	/*
	*	Binds a storage buffer to a render data table.
	*/
	void BindStorageBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, BufferHandle buffer) const NOEXCEPT;

	/*
	*	Binds a storage image to a render data table.
	*/
	void BindStorageImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, OpaqueHandle image) const NOEXCEPT;

	/*
	*	Binds a uniform buffer to a render data table.
	*/
	void BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, BufferHandle buffer) const NOEXCEPT;

	/*
	*	Destroys a render data table.
	*/
	void DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a render target.
	*/
	void CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates and returns a sampler.
	*/
	void CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a texture 2D.
	*/
	void CreateTexture2D(const TextureData &textureData, Texture2DHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Destroys a texture 2D.
	*/
	void DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a texture 3D.
	*/
	void CreateTexture3D(const TextureData& textureData, Texture3DHandle* const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates a texture cube.
	*/
	void CreateTextureCube(const TextureCubeData& data, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Initializes a pipeline
	*/
	void InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT;

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
	RenderDataTableHandle GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) const NOEXCEPT;

private:

	//The number of noise textures.
	static constexpr uint8 NUMBER_OF_NOISE_TEXTURES{ 64 };

	//The noise texture size.
	static constexpr uint8 NOISE_TEXTURE_SIZE{ 64 };

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
	StaticArray<RenderDataTableLayoutHandle, UNDERLYING(CommonRenderDataTableLayout::NumberOfCommonRenderDataTableLayouts)> _CommonRenderDataTableLayouts;

	//The default texture 2D.
	Texture2DHandle _DefaultTexture2D;

	//The dynamic uniform data.
	DynamicUniformData _DynamicUniformData;

	//The current jitter index.
	uint8 _CurrentJitterIndex{ 0 };

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

#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
	//The rendering reference system.
	RenderingReferenceSystem _RenderingReferenceSystem;
#endif

	//The noise textures.
	StaticArray<const Texture2DResource *RESTRICT, NUMBER_OF_NOISE_TEXTURES> _NoiseTextures;

	//The active noise texture index.
	uint8 _ActiveNoiseTextureIndex{ 0 };

	//The Hammersley hemisphere samples uniform buffer.
	BufferHandle _HammersleyHemisphereSamplesUniformBuffer;

	/*
	*	Pre-initializes the rendering system.
	*/
	void PreInitialize() NOEXCEPT;

	/*
	*	Post-initializes the rendering system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Begins a frame.
	*/
	void BeginFrame() NOEXCEPT;

	/*
	*	Ends a frame.
	*/
	void EndFrame() NOEXCEPT;

	/*
	*	Pre-releases the rendering system.
	*/
	void PreRelease() NOEXCEPT;

	/*
	*	Post-releases the rendering system.
	*/
	void PostRelease() NOEXCEPT;

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
	*	Initializes the default texture.
	*/
	void InitializeDefaultTexture() NOEXCEPT;

	/*
	*	Initializes the Hammersley hemisphere samples uniform buffer.
	*/
	void InitializeHammersleyHemisphereSamplesUniformBuffer() NOEXCEPT;

	/*
	*	Initializes the noise textures.
	*/
	void InitializeNoiseTextures() NOEXCEPT;

	/*
	*	Post-initializes the global render data.
	*/
	void PostInitializeGlobalRenderData() NOEXCEPT;

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