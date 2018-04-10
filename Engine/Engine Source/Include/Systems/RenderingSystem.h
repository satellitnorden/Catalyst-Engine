#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

//Forward declarations.
class CameraEntity;
class CPUTexture2D;
class EnvironmentMaterial;
class EnvironmentMaterialData;
class InstancedPhysicalEntity;
class Matrix4;
class PhysicalMaterial;
class PhysicalMaterialData;
class PhysicalModel;
class PhysicalModelData;
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
class VegetationTransformation;
class WaterEntity;
class WaterMaterialData;
class WaterMaterial;
class WaterUniformData;

class RenderingSystem final
{

	//System declaration.
	DECLARE_SYSTEM(RenderingSystem);

public:

	/*
	*	Default constructor.
	*/
	RenderingSystem() NOEXCEPT;

	/*
	*	Initializes the rendering system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the rendering system synchronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the rendering system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Creates an environment material.
	*/
	void CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT;

	/*
	*	Creates a terrain material.
	*/
	void CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT;

	/*
	*	Creates a physical model.
	*/
	void CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT;

	/*
	*	Creates a physical material.
	*/
	void CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT;

	/*
	*	Creates a vegetation material.
	*/
	void CreateVegetationMaterial(const VegetationMaterialData &vegetationMaterialData, VegetationMaterial &vegetationMaterial) const NOEXCEPT;

	/*
	*	Creates a water material.
	*/
	void CreateWaterMaterial(const WaterMaterialData &waterMaterialData, WaterMaterial &waterMaterial) const NOEXCEPT;

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
	void InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations) const NOEXCEPT;

	/*
	*	Initializes a water entity.
	*/
	void InitializeWaterEntity(const WaterEntity *const RESTRICT waterEntity, const WaterMaterial &waterMaterial, const WaterUniformData &waterUniformData) const NOEXCEPT;

	/*
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT;

	/*
	*	Sets the environment material.
	*/
	void SetEnvironmentMaterial(const EnvironmentMaterial &newEnvioronmentMaterial) NOEXCEPT;

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

};
