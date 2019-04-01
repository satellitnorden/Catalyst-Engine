#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/Map.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Engine/GlobalRenderData.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/Resolution.h>
#include <Rendering/Engine/TextureData.h>

//Forward declarations.
class AxisAlignedBoundingBox;
class CatalystProjectRenderingConfiguration;
class Entity;
class RenderPass;
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
	RenderingSystem() NOEXCEPT { }

	/*
	*	Initializes the rendering system.
	*/
	void InitializeSystem(const CatalystProjectRenderingConfiguration &configuration) NOEXCEPT;

	/*
	*	Post-initializes the rendering system.
	*/
	void PostInitializeSystem();

	/*
	*	Updates the rendering system synchronously.
	*/
	void UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Releases the rendering system.
	*/
	void ReleaseSystem() NOEXCEPT;

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
	*	Returns the number of frame buffers
	*/
	uint8 GetNumberOfFrameBuffers() const NOEXCEPT;

	/*
	*	Returns the current frame buffer index.
	*/
	uint8 GetCurrentFrameBufferIndex() const NOEXCEPT;

	/*
	*	Returns the render passes.
	*/
	const StaticArray<RenderPass *RESTRICT, UNDERLYING(RenderPassSubStage::NumberOfRenderPassSubStages)>& GetRenderPasses() const
	{
		return _RenderPasses;
	}

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
	void DestroyConstantBuffer(ConstantBufferHandle handle) const NOEXCEPT;

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
	void DestroyRenderDataTable(RenderDataTableHandle handle) const NOEXCEPT;

	/*
	*	Creates a render target.
	*/
	void CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT;

	/*
	*	Creates and returns a sampler.
	*/
	SamplerHandle CreateSampler(const SamplerProperties &properties) const NOEXCEPT;

	/*
	*	Creates and returns a texture 2D given the texture data.
	*/
	Texture2DHandle CreateTexture2D(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Destroys a texture 2D
	*/
	void DestroyTexture2D(Texture2DHandle handle) const NOEXCEPT;

	/*
	*	Creates a texture cube.
	*/
	TextureCubeHandle CreateTextureCube(const float *const RESTRICT data, const Resolution resolution) const NOEXCEPT;

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
	void DestroyUniformBuffer(UniformBufferHandle handle) const NOEXCEPT;

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

	/*
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeRenderPassInitialization(RenderPass *const RESTRICT _RenderPass) NOEXCEPT;

private:

	//The resolution.
	Resolution _Resolution;

	//The scaled resolution.
	Resolution _ScaledResolution;

	//The directional shadow map resolution.
	Resolution _DirectionalShadowMapResolution;

	//The global render data.
	GlobalRenderData _GlobalRenderData;

	//Container for all render passes.
	StaticArray<RenderPass *RESTRICT, UNDERLYING(RenderPassSubStage::NumberOfRenderPassSubStages)> _RenderPasses;

	//Container for all depth buffers.
	StaticArray<DepthBufferHandle, UNDERLYING(DepthBuffer::NumberOfDepthBuffers)> _DepthBuffers;

	//Container for all render targets.
	StaticArray<RenderTargetHandle, UNDERLYING(RenderTarget::NumberOfRenderTargets)> _RenderTargets;

	//Container for all samplers.
	StaticArray<SamplerHandle, UNDERLYING(Sampler::NumberOfSamplers)> _Samplers;

	//Container for all common render data table layouts.
	StaticArray<RenderDataTableLayoutHandle, UNDERLYING(CommonRenderDataTableLayout::NumberOfCommonRenderDataTableLayouts)> _CommonRenderDataTableLayouts;

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
	*	Registers all render passes.
	*/
	void RegisterRenderPasses() NOEXCEPT;

	/*
	*	Initializes all render passes.
	*/
	void InitializeRenderPasses() NOEXCEPT;

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