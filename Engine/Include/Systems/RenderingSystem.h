#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine/EnvironmentMaterial.h>
#include <Rendering/Engine/OceanMaterial.h>
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/Surface.h>

//Forward declarations.
class CameraEntity;
class CPUTexture2D;
class EnvironmentMaterialData;
class InstancedPhysicalEntity;
class ParticleMaterialData;
class ParticleSystemEntity;
class ParticleSystemProperties;
class PhysicalMaterial;
class PhysicalMaterialData;
class PhysicalModelData;
class RenderPass;
class Resolution;
class StaticPhysicalEntity;
class TerrainEntity;
class TerrainInitializationData;
class TerrainMaterial;
class TerrainMaterialData;
class TerrainUniformData;
class TextureData;
class Vector3;
class VegetationEntity;
class VegetationMaterial;
class VegetationMaterialData;
class VegetationProperties;
class VegetationTransformation;
class WaterMaterialData;
class WaterUniformData;

class RenderingSystem final
{

public:

	//System declaration.
	DECLARE_SINGLETON(RenderingSystem);

	//Enumeration covering all common particle materials.
	enum class CommonParticleMaterial : uint8
	{
		WhiteCircle,
		NumberOfCommonParticleMaterials
	};

	//Enumeration covering all common physical materials.
	enum class CommonPhysicalMaterial : uint8
	{
		Red,
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
	void InitializeSystem() NOEXCEPT;

	/*
	*	Pre-updates the rendering system synchronously.
	*/
	void PreUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Updates the rendering system synchronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the rendering system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the main surface.
	*/
	RESTRICTED Surface *const RESTRICT GetMainSurface() { return &mainSurface; }

	/*
	*	Returns the render passes.
	*/
	const StaticArray<RenderPass *RESTRICT, INDEX(RenderPassSubStage::NumberOfRenderPassStages)>& GetRenderPasses() const { return renderPasses; }

	/*
	*	Returns the resolution.
	*/
	Resolution GetResolution() const NOEXCEPT;

	/*
	*	Returns the current frame index.
	*/
	uint8 GetCurrentFrameIndex() const NOEXCEPT;

	/*
	*	Creates a constant buffer.
	*/
	ConstantBufferHandle CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT;

	/*
	*	Returns the active camera, const.
	*/
	RESTRICTED const CameraEntity *const RESTRICT GetActiveCamera() const NOEXCEPT { return activeCamera; }

	/*
	*	Returns the active camera, const.
	*/
	RESTRICTED CameraEntity *const RESTRICT GetActiveCamera() NOEXCEPT { return activeCamera; }

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(CameraEntity *const RESTRICT newActiveCamera) NOEXCEPT { activeCamera = newActiveCamera; }

	/*
	*	Returns the projection matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetProjectionMatrix() const NOEXCEPT { return &projectionMatrix; }

	/*
	*	Returns the camera matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetCameraMatrix() const NOEXCEPT { return &cameraMatrix; }

	/*
	*	Returns the view matrix.
	*/
	RESTRICTED const Matrix4 *const RESTRICT GetViewMatrix() const NOEXCEPT { return &viewMatrix; }

	/*
	*	Returns the given common particle material.
	*/
	const ParticleMaterial& GetCommonParticleMaterial(const CommonParticleMaterial commonParticlelMaterial) const NOEXCEPT { return commonParticleMaterials[INDEX(commonParticlelMaterial)]; }

	/*
	*	Returns the given common physical material.
	*/
	const PhysicalMaterial& GetCommonPhysicalMaterial(const CommonPhysicalMaterial commonPhysicalMaterial) const NOEXCEPT { return commonPhysicalMaterials[INDEX(commonPhysicalMaterial)]; }

	/*
	*	Returns the given common physical model.
	*/
	const PhysicalModel& GetCommonPhysicalModel(const CommonPhysicalModel commonPhysicalModel) const NOEXCEPT { return commonPhysicalModels[INDEX(commonPhysicalModel)]; }

	/*
	*	Returns the default ocean material.
	*/
	const OceanMaterial& GetDefaultOceanMaterial() const NOEXCEPT { return defaultOceanMaterial; }

	/*
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT;

	/*
	*	Returns the current dynamic uniform data descriptor set.
	*/
	RenderDataTableHandle GetCurrentDynamicUniformDataDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the current environment data descriptor set.
	*/
	RenderDataTableHandle GetCurrentEnvironmentDataDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the current ocean descriptor set.
	*/
	RenderDataTableHandle GetCurrentOceanDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the given render data table.
	*/
	RenderDataTableHandle GetRenderDataTable(const RenderDataTable renderDataTable) const NOEXCEPT;

	/*
	*	Creates an environment material.
	*/
	void CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT;

	/*
	*	Creates an ocean material.
	*/
	void CreateOceanMaterial(const WaterMaterialData &waterMaterialData, OceanMaterial &oceanMaterial) const NOEXCEPT;

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
	*	Creates a vegetation material.
	*/
	void CreateVegetationMaterial(const VegetationMaterialData &vegetationMaterialData, VegetationMaterial &vegetationMaterial) const NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(const TerrainEntity *const RESTRICT entity, const TerrainInitializationData *const RESTRICT data) const NOEXCEPT;

	/*
	*	Terminates a terrain entity.
	*/
	void TerminateTerrainEntity(const TerrainEntity *const RESTRICT entity) const NOEXCEPT;

	/*
	*	Initializes a static physical entity.
	*/
	void InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT;

	/*
	*	Initializes an instanced physical entity.
	*/
	void InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT;

	/*
	*	Initializes a vegetation entity.
	*/
	void InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT;

	/*
	*	Initializes a particle system entity.
	*/
	void InitializeParticleSystemEntity(const ParticleSystemEntity &entity, const ParticleMaterial &material, const ParticleSystemProperties &properties) const NOEXCEPT;

	/*
	*	Creates and returns a texture 2D given the texture data.
	*/
	Texture2DHandle CreateTexture2D(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Destroys a texture 2D
	*/
	void DestroyTexture2D(Texture2DHandle texture) const NOEXCEPT;

	/*
	*	Destroys a render data table.
	*/
	void DestroyRenderDataTable(RenderDataTableHandle renderDataTable) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

private:

	//The main surface.
	Surface mainSurface;

	//Container for all render passes.
	StaticArray<RenderPass *RESTRICT, INDEX(RenderPassSubStage::NumberOfRenderPassStages)> renderPasses;

	//Container for all common particle materials.
	StaticArray<ParticleMaterial, INDEX(CommonParticleMaterial::NumberOfCommonParticleMaterials)> commonParticleMaterials;

	//Container for all common physical materials.
	StaticArray<PhysicalMaterial, INDEX(CommonPhysicalMaterial::NumberOfCommonPhysicalMaterials)> commonPhysicalMaterials;

	//Container for all common physical models.
	StaticArray<PhysicalModel, INDEX(CommonPhysicalModel::NumberOfCommonPhysicalModels)> commonPhysicalModels;

	//The default night environment material.
	EnvironmentMaterial defaultNightEnvironmentMaterial;

	//The default day environment material.
	EnvironmentMaterial defaultDayEnvironmentMaterial;

	//The default ocean material.
	OceanMaterial defaultOceanMaterial;

	//The active camera.
	CameraEntity *RESTRICT activeCamera{ nullptr };

	//The projection matrix.
	Matrix4 projectionMatrix;

	//The camera matrix.
	Matrix4 cameraMatrix;

	//The view matrix.
	Matrix4 viewMatrix;

	/*
	*	Registers all render passes.
	*/
	void RegisterRenderPasses() NOEXCEPT;

	/*
	*	Initializes all render passes.
	*/
	void InitializeRenderPasses() NOEXCEPT;

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
	*	Initializes all default assets.
	*/
	void InitializeDefaultAssets() NOEXCEPT;

	/*
	*	Updates the matrices.
	*/
	void UpdateMatrices() NOEXCEPT;

};
