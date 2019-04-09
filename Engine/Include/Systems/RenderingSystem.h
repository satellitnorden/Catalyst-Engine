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
#include <Rendering/Native/GlobalRenderData.h>
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>
#include <Rendering/Native/Pipelines/Core/Pipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Forward declarations.
class AxisAlignedBoundingBox;
class CatalystProjectRenderingConfiguration;
class Entity;
class Resolution;

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
	*	Post-initializes the rendering system.
	*/
	void PostInitializeSystem();

	/*
	*	Updates the rendering system.
	*/
	void UpdateSystem(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Releases the rendering system.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the resolution.
	*/
	Resolution GetResolution() const NOEXCEPT
	{
		return _Resolution;
	}

	/*
	*	Returns the scaled resolution.
	*/
	Resolution GetScaledResolution() const NOEXCEPT
	{
		return _ScaledResolution;
	}

	/*
	*	Returns the directional shadow map resolution.
	*/
	Resolution GetDirectionalShadowMapResolution() const NOEXCEPT
	{
		return _DirectionalShadowMapResolution;
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
	*	Returns the given depth buffer.
	*/
	DepthBufferHandle GetDepthBuffer(const DepthBuffer depthBuffer) const NOEXCEPT
	{
		return _DepthBuffers[UNDERLYING(depthBuffer)];
	}

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
	*	Creates a bottom level acceleration structure.
	*/
	void CreateBottomLevelAccelerationStructure(const BufferHandle &buffer,
												const uint32 verticesOffset,
												const uint32 numberOfVertices,
												const uint32 indicesOffset,
												const uint32 numberOfIndices,
												AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT;

	/*
	*	Creates a top level acceleration structure.
	*/
	void CreateTopLevelAccelerationStructure(const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instanceData, AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT;

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
	*	Creates a texture cube.
	*/
	void CreateTextureCube(const float *const RESTRICT data, const Resolution resolution, TextureCubeHandle *const RESTRICT) const NOEXCEPT;

	/*
	*	Initializes a render pass
	*/
	void InitializeRenderPass(RenderPass *const RESTRICT renderPass) const NOEXCEPT;

	/*
	*	Initializes a pipeline
	*/
	void InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT;

	/*
	*	Returns the global render data table.
	*/
	RenderDataTableHandle GetGlobalRenderDataTable() const NOEXCEPT;

	/*
	*	Adds a terrain height texture to the global render data and returns it's index.
	*/
	uint8 AddTerrainHeightTextureToGlobalRenderData(Texture2DHandle texture) NOEXCEPT;

	/*
	*	Returns a terrain height texture to the global render data and marks it's index as available.
	*/
	void ReturnTerrainHeightTextureToGlobalRenderData(const uint8 index) NOEXCEPT;

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

	//The resolution.
	Resolution _Resolution;

	//The scaled resolution.
	Resolution _ScaledResolution;

	//The directional shadow map resolution.
	Resolution _DirectionalShadowMapResolution;

	//The global render data.
	GlobalRenderData _GlobalRenderData;

	//Container for all depth buffers.
	StaticArray<DepthBufferHandle, UNDERLYING(DepthBuffer::NumberOfDepthBuffers)> _DepthBuffers;

	//Container for all render targets.
	StaticArray<RenderTargetHandle, UNDERLYING(RenderTarget::NumberOfRenderTargets)> _RenderTargets;

	//Container for all samplers.
	StaticArray<SamplerHandle, UNDERLYING(Sampler::NumberOfSamplers)> _Samplers;

	//Container for all common render data table layouts.
	StaticArray<RenderDataTableLayoutHandle, UNDERLYING(CommonRenderDataTableLayout::NumberOfCommonRenderDataTableLayouts)> _CommonRenderDataTableLayouts;

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
	*	Initializes all depth buffers.
	*/
	void InitializeDepthBuffers() NOEXCEPT;

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
	*	Updates the global render data.
	*/
	void UpdateGlobalRenderData() NOEXCEPT;

	/*
	*	Updates the dynamic uniform data.
	*/
	void UpdateDynamicUniformData(const uint8 currentFrameBufferIndex) NOEXCEPT;

	/*
	*	Updates the global textures.
	*/
	void UpdateGlobalTextures(const uint8 currentFrameBufferIndex) NOEXCEPT;

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

};