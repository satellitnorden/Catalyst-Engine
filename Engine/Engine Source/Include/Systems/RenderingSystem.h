#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Engine Layer/RenderingCore.h>

//Forward declarations.
class CameraEntity;
class CPUTexture2D;
class EnvironmentMaterial;
class EnvironmentMaterialData;
class InstancedPhysicalEntity;
class OceanMaterial;
class ParticleMaterial;
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

	//Enumeration covering all common physical model.
	enum class CommonPhysicalModel : uint8
	{
		Plane,
		NumberOfCommonPhysicalModels
	};

	/*
	*	Default constructor.
	*/
	RenderingSystem() NOEXCEPT = default;

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
	*	Returns the render resolution.
	*/
	Resolution GetRenderResolution() const NOEXCEPT;

	/*
	*	Returns the current frame index.
	*/
	uint8 GetCurrentFrameIndex() const NOEXCEPT;

	/*
	*	Creates a constant buffer.
	*/
	ConstantBufferHandle CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT;

	/*
	*	Returns the active camera.
	*/
	RESTRICTED const CameraEntity *const RESTRICT GetActiveCamera() const NOEXCEPT { return activeCamera; }

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(const CameraEntity *const RESTRICT newActiveCamera) NOEXCEPT { activeCamera = newActiveCamera; }

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
	*	Returns a common physical model.
	*/
	const PhysicalModel& GetCommonPhysicalModel(const CommonPhysicalModel commonPhysicalModel) const NOEXCEPT { return commonPhysicalModels[INDEX(commonPhysicalModel)]; }

	/*
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT;

	/*
	*	Returns the current dynamic uniform data descriptor set.
	*/
	DescriptorSetHandle GetCurrentDynamicUniformDataDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the current environment data descriptor set.
	*/
	DescriptorSetHandle GetCurrentEnvironmentDataDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the current ocean descriptor set.
	*/
	DescriptorSetHandle GetCurrentOceanDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the directional shadow descriptor set.
	*/
	DescriptorSetHandle GetDirectionalShadowDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the lighting descriptor set.
	*/
	DescriptorSetHandle GetLightingDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the post processing descriptor set.
	*/
	DescriptorSetHandle GetPostProcessingDescriptorSet() const NOEXCEPT;

	/*
	*	Returns the shadow blur descriptor set.
	*/
	DescriptorSetHandle GetShadowBlurDescriptorSet() const NOEXCEPT;

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
	void InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture2D &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT;

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
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

	/*
	*	Sets the post processing blur amount.
	*/
	void SetPostProcessingBlurAmount(const float newBlurAmount) NOEXCEPT;

	/*
	*	Sets the post processing chromatic aberration amount.
	*/
	void SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) NOEXCEPT;

	/*
	*	Sets the post processing saturation.
	*/
	void SetPostProcessingSaturation(const float newSaturation) NOEXCEPT;

	/*
	*	Sets the post processing sharpen amount.
	*/
	void SetPostProcessingSharpenAmount(const float newSharpenAmount) NOEXCEPT;

private:

	//Container for all render passes.
	StaticArray<RenderPass *RESTRICT, INDEX(RenderPassStage::NumberOfRenderPassStages)> renderPasses;

	//Container for all common physical models.
	StaticArray<PhysicalModel, INDEX(CommonPhysicalModel::NumberOfCommonPhysicalModels)> commonPhysicalModels;

	//The active camera.
	const CameraEntity *RESTRICT activeCamera{ nullptr };

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
	*	Initializes the common physical models.
	*/
	void InitializeCommonPhysicalModels() NOEXCEPT;

	/*
	*	Updates the matrices.
	*/
	void UpdateMatrices() NOEXCEPT;

};
