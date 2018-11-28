#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/Map.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine/EnvironmentMaterial.h>
#include <Rendering/Engine/GlobalRenderData.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Rendering/Engine/OceanMaterial.h>
#endif
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/Resolution.h>

//Forward declarations.
class AxisAlignedBoundingBox;
class CatalystProjectRenderingConfiguration;
class CPUTexture2D;
class DynamicPhysicalInitializationData;
class Entity;
class EnvironmentMaterialData;
class GrassVegetationMaterial;
class GrassVegetationMaterialData;
class GrassVegetationModel;
class GrassVegetationModelData;
#if defined(CATALYST_ENABLE_OCEAN)
class OceanMaterialData;
#endif
class ParticleMaterialData;
class ParticleSystemEntity;
class ParticleSystemInitializationData;
class ParticleSystemProperties;
class PhysicalMaterial;
class PhysicalMaterialData;
class PhysicalModelData;
class RenderPass;
class Resolution;
class TextureData;

class RenderingSystem final
{

public:

	//System declaration.
	DECLARE_SINGLETON(RenderingSystem);

	//Enumeration covering all common environment materials.
	enum class CommonEnvironmentMaterial : uint8
	{
		Black,
		Day,
		Night,

		NumberOfCommonEnvironmentMaterials
	};

#if defined(CATALYST_ENABLE_OCEAN)
	//Enumeration covering all common ocean materials.
	enum class CommonOceanMaterial : uint8
	{
		Ocean,

		NumberOfCommonOceanMaterials
	};
#endif

	//Enumeration covering all common particle materials.
	enum class CommonParticleMaterial : uint8
	{
		WhiteCircle,

		NumberOfCommonParticleMaterials
	};

	//Enumeration covering all common physical materials.
	enum class CommonPhysicalMaterial : uint8
	{
		Black,
		Red,
		Teal,

		NumberOfCommonPhysicalMaterials
	};

	//Enumeration covering all common physical models.
	enum class CommonPhysicalModel : uint8
	{
		Cube,
		Octahedron,
		Plane,

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
	*	Updates the rendering system synchronously during the rendering update phase.
	*/
	void RenderingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

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
	*	Returns the given common environment material.
	*/
	const EnvironmentMaterial& GetCommonEnvironmentMaterial(const CommonEnvironmentMaterial commonEnvironmentMaterial) const NOEXCEPT
	{
		return _CommonEnvironmentMaterials[UNDERLYING(commonEnvironmentMaterial)];
	}

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Returns the given common ocean material.
	*/
	const OceanMaterial& GetCommonOceanMaterial(const CommonOceanMaterial commonOceanMaterial) const NOEXCEPT
	{
		return _CommonOceanMaterials[UNDERLYING(commonOceanMaterial)];
	}
#endif

	/*
	*	Returns the given common particle material.
	*/
	const ParticleMaterial& GetCommonParticleMaterial(const CommonParticleMaterial commonParticleMaterial) const NOEXCEPT
	{
		return _CommonParticleMaterials[UNDERLYING(commonParticleMaterial)];
	}

	/*
	*	Returns the given common physical material.
	*/
	const PhysicalMaterial& GetCommonPhysicalMaterial(const CommonPhysicalMaterial commonPhysicalMaterial) const NOEXCEPT
	{
		return _CommonPhysicalMaterials[UNDERLYING(commonPhysicalMaterial)];
	}

	/*
	*	Returns the given common physical model.
	*/
	const PhysicalModel& GetCommonPhysicalModel(const CommonPhysicalModel commonPhysicalModel) const NOEXCEPT
	{
		return _CommonPhysicalModels[UNDERLYING(commonPhysicalModel)];
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
	*	Creates an environment material.
	*/
	void CreateEnvironmentMaterial(const EnvironmentMaterialData &data, EnvironmentMaterial &material) NOEXCEPT;

	/*
	*	Creates a grass vegetation material.
	*/
	void CreateGrassVegetationMaterial(const GrassVegetationMaterialData &data, GrassVegetationMaterial &material) NOEXCEPT;

	/*
	*	Creates a grass vegetation model.
	*/
	void CreateGrassVegetationModel(const GrassVegetationModelData &data, GrassVegetationModel &model) NOEXCEPT;

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Creates an ocean material.
	*/
	void CreateOceanMaterial(const OceanMaterialData &data, OceanMaterial &material) const NOEXCEPT;
#endif

	/*
	*	Creates a physical model.
	*/
	void CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT;

	/*
	*	Creates a particle material.
	*/
	void CreateParticleMaterial(const ParticleMaterialData &particleMaterialData, ParticleMaterial &particleMaterial) const NOEXCEPT;

	/*
	*	Creates a physical material.
	*/
	void CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) NOEXCEPT;

	/*
	*	Initializes a dynamic physical entity.
	*/
	void InitializeDynamicPhysicalEntity(const Entity *const RESTRICT entity, const DynamicPhysicalInitializationData *const RESTRICT data) const NOEXCEPT;

	/*
	*	Initializes a particle system entity.
	*/
	void InitializeParticleSystemEntity(const Entity *const RESTRICT entity, const ParticleSystemInitializationData *const RESTRICT data) const NOEXCEPT;

	/*
	*	Terminates a particle system entity.
	*/
	void TerminateParticleSystemEntity(const Entity *const RESTRICT entity) const NOEXCEPT;

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

	//Container for all common environment materials.
	StaticArray<EnvironmentMaterial, UNDERLYING(CommonEnvironmentMaterial::NumberOfCommonEnvironmentMaterials)> _CommonEnvironmentMaterials;

#if defined(CATALYST_ENABLE_OCEAN)
	//Container for all common ocean materials.
	StaticArray<OceanMaterial, UNDERLYING(CommonOceanMaterial::NumberOfCommonOceanMaterials)> _CommonOceanMaterials;
#endif

	//Container for all common particle materials.
	StaticArray<ParticleMaterial, UNDERLYING(CommonParticleMaterial::NumberOfCommonParticleMaterials)> _CommonParticleMaterials;

	//Container for all common physical materials.
	StaticArray<PhysicalMaterial, UNDERLYING(CommonPhysicalMaterial::NumberOfCommonPhysicalMaterials)> _CommonPhysicalMaterials;

	//Container for all common physical models.
	StaticArray<PhysicalModel, UNDERLYING(CommonPhysicalModel::NumberOfCommonPhysicalModels)> _CommonPhysicalModels;

	//The default night environment material.
	EnvironmentMaterial _DefaultNightEnvironmentMaterial;

	//The default day environment material.
	EnvironmentMaterial _DefaultDayEnvironmentMaterial;

	/*
	*	Initializes the global render data.
	*/
	void InitializeGlobalRenderData() NOEXCEPT;

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
	*	Initializes the common environment materials.
	*/
	void InitializeCommonEnvironmentMaterials() NOEXCEPT;

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Initializes the common ocean materials.
	*/
	void InitializeCommonOceanMaterials() NOEXCEPT;
#endif

	/*
	*	Initializes the common particle materials.
	*/
	void InitializeCommonParticleMaterials() NOEXCEPT;

	/*
	*	Initializes the common physical materials.
	*/
	void InitializeCommonPhysicalMaterials() NOEXCEPT;

	/*
	*	Initializes the common physical models.
	*/
	void InitializeCommonPhysicalModels() NOEXCEPT;

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