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
class TerrainMaterial;
class TerrainMaterialData;
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
	*	Returns the current frame index.
	*/
	uint8 GetCurrentFrameIndex() const NOEXCEPT;

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
	SamplerHandle GetSampler(const SamplerProperties &properties) const NOEXCEPT;

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
	*	Creates a constant buffer.
	*/
	ConstantBufferHandle CreateConstantBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT;

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
	*	Updates a render data table.
	*/
	void UpdateRenderDataTable(const RenderDataTableUpdateInformation information, RenderDataTableHandle handle) const NOEXCEPT;

	/*
	*	Binds a combined image sampler to a render data table.
	*	Accepts render target, texture 2D and texture cube handles. 
	*/
	void BindCombinedImageSamplerToRenderDataTable(const uint32 binding, RenderDataTableHandle renderDataTable, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT;

	/*
	*	Binds a uniform buffer to a render data table.
	*/
	void BindUniformBufferToRenderDataTable(const uint32 binding, RenderDataTableHandle renderDataTable, UniformBufferHandle uniformBuffer) const NOEXCEPT;

	/*
	*	Destroys a render data table.
	*/
	void DestroyRenderDataTable(RenderDataTableHandle handle) const NOEXCEPT;

	/*
	*	Creates a render target.
	*/
	void CreateRenderTarget(const Resolution resolution, const TextureFormat format, const TextureFilter filter, const AddressMode addressMode, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT;

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
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

	/*
	*	Uploads data to a uniform buffer.
	*/
	void UploadDataToUniformBuffer(UniformBufferHandle handle, const void *const RESTRICT data) const NOEXCEPT;

	/*
	*	Destroys a uniform buffer.
	*/
	void DestroyUniformBuffer(UniformBufferHandle handle) const NOEXCEPT;

	/*
	*	Returns the current dynamic uniform data render data table.
	*/
	RenderDataTableHandle GetCurrentDynamicUniformDataRenderDataTable() const NOEXCEPT;

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
	void CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT;

	/*
	*	Creates a terrain material.
	*/
	void CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT;

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

	//Container for all render passes.
	StaticArray<RenderPass *RESTRICT, UNDERLYING(RenderPassSubStage::NumberOfRenderPassSubStages)> _RenderPasses;

	//Container for all depth buffers.
	StaticArray<DepthBufferHandle, UNDERLYING(DepthBuffer::NumberOfDepthBuffers)> _DepthBuffers;

	//Container for all render targets.
	StaticArray<RenderTargetHandle, UNDERLYING(RenderTarget::NumberOfRenderTargets)> _RenderTargets;

	//Container for all samplers.
	Map<SamplerProperties, SamplerHandle> _Samplers;

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

};
