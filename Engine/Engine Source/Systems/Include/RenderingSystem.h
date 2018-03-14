#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <RenderingCore.h>

//Forward declarations.
class CameraEntity;
class CPUTexture4;
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
	*	Post-initializes the rendering system.
	*/
	void PostInitializeSystem() NOEXCEPT;

	/*
	*	Updates the rendering system synchronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the rendering system.
	*/
	void ReleaseSystem() NOEXCEPT;

#if !defined(CATALYST_FINAL)
	/*
	*	Constructs an environment material.
	*/
	void ConstructEnvironmentMaterial(float *const RESTRICT data, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, DynamicArray<float> &diffuseData, DynamicArray<float> &diffuseIrradianceData) NOEXCEPT;
#endif

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
	*	Creates a water material.
	*/
	void CreateWaterMaterial(const WaterMaterialData &waterMaterialData, WaterMaterial &waterMaterial) const NOEXCEPT;

	/*
	*	Initializes a terrain entity.
	*/
	void InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture4 &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT;

	/*
	*	Initializes a static physical entity.
	*/
	void InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT;

	/*
	*	Initializes an instanced physical entity.
	*/
	void InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT;

	/*
	*	Initializes a water entity.
	*/
	void InitializeWaterEntity(const WaterEntity *const RESTRICT waterEntity, const WaterMaterial &waterMaterial, const WaterUniformData &waterUniformData) const NOEXCEPT;

	/*
	*	Creates and returns a 2D texture given the texture data.
	*/
	Texture2DHandle Create2DTexture(const TextureData &textureData) const NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	TextureCubeMapHandle CreateCubeMapTexture(const char *RESTRICT frontTexturePath, const char *RESTRICT backTexturePath, const char *RESTRICT upTexturePath, const char *RESTRICT downTexturePath, const char *RESTRICT rightTexturePath, const char *RESTRICT leftTexturePath) const NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	UniformBufferHandle CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT;

	/*
	*	Sets the active camera.
	*/
	void SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT;

	/*
	*	Sets the active sky box cube map texture.
	*/
	void SetActiveSkyBox(TextureCubeMapHandle newSkyBox) NOEXCEPT;

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
