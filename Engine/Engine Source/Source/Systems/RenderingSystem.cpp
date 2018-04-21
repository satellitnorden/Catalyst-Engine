//Header file.
#include <Systems/RenderingSystem.h>

//Rendering.
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Translation Layer/Vulkan/VulkanRenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(RenderingSystem);

//Define the current rendering system.
#if defined(CATALYST_WINDOWS)
	#define CURRENT_RENDERING_SYSTEM VulkanRenderingSystem
#endif

/*
*	Initializes the rendering system.
*/
void RenderingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeSystem();
}

/*
*	Updates the rendering system synchronously.
*/
void RenderingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Update the current rendering system synchronously.
	CURRENT_RENDERING_SYSTEM::Instance->UpdateSystemSynchronous();
}

/*
*	Releases the rendering system.
*/
void RenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->ReleaseSystem();
}

/*
*	Creates an environment material.
*/
void RenderingSystem::CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT
{
	//Create the environment material via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateEnvironmentMaterial(environmentMaterialData, environmentMaterial);
}

/*
*	Creates a terrain material.
*/
void RenderingSystem::CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Create the terrain material via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateTerrainMaterial(terrainMaterialData, terrainMaterial);
}

/*
*	Creates a physical model.
*/
void RenderingSystem::CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT
{
	//Create the physical model via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreatePhysicalModel(physicalModelData, physicalModel);
}

/*
*	Creates a particle material.
*/
void RenderingSystem::CreateParticleMaterial(const ParticleMaterialData &particleMaterialData, ParticleMaterial &particleMaterial) const NOEXCEPT
{
	//Create the particle material via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateParticleMaterial(particleMaterialData, particleMaterial);
}

/*
*	Creates a physical material.
*/
void RenderingSystem::CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT
{
	//Create the physical material via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreatePhysicalMaterial(physicalMaterialData, physicalMaterial);
}

/*
*	Creates a vegetation material.
*/
void RenderingSystem::CreateVegetationMaterial(const VegetationMaterialData &vegetationMaterialData, VegetationMaterial &vegetationMaterial) const NOEXCEPT
{
	//Create the vegetation model via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateVegetationMaterial(vegetationMaterialData, vegetationMaterial);
}

/*
*	Creates a water material.
*/
void RenderingSystem::CreateWaterMaterial(const WaterMaterialData &waterMaterialData, WaterMaterial &waterMaterial) const NOEXCEPT
{
	//Create the water material via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateWaterMaterial(waterMaterialData, waterMaterial);
}

/*
*	Initializes a terrain entity.
*/
void RenderingSystem::InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture2D &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT
{
	//Initialize the terrain entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeTerrainEntity(terrainEntity, terrainPlaneResolution, terrainProperties, terrainUniformData, layerWeightsTexture, terrainMaterial);
}

/*
*	Initializes a static physical entity.
*/
void RenderingSystem::InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT
{
	//Initialize the static physical entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeStaticPhysicalEntity(staticPhysicalEntity, model, position, rotation, scale);
}

/*
*	Initializes an instanced physical entity.
*/
void RenderingSystem::InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Initialize the instanced physical entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeInstancedPhysicalEntity(entity, model, transformations);
}

/*
*	Initializes a vegetation entity.
*/
void RenderingSystem::InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT
{
	//Initialize the vegetation entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeVegetationEntity(entity, material, transformations, properties);
}

/*
*	Initializes a particle system entity.
*/
void RenderingSystem::InitializeParticleSystemEntity(const ParticleSystemEntity &entity, const ParticleMaterial &material, const ParticleSystemProperties &properties) const NOEXCEPT
{
	//Initialize the particle system entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeParticleSystemEntity(entity, material, properties);
}

/*
*	Initializes a water entity.
*/
void RenderingSystem::InitializeWaterEntity(const WaterEntity *const RESTRICT waterEntity, const WaterMaterial &waterMaterial, const WaterUniformData &waterUniformData) const NOEXCEPT
{
	//Initialize the water entity via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->InitializeWaterEntity(waterEntity, waterMaterial, waterUniformData);
}

/*
*	Creates and returns a 2D texture given the texture data.
*/
Texture2DHandle RenderingSystem::Create2DTexture(const TextureData &textureData) const NOEXCEPT
{
	//Create the 2D texture via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->Create2DTexture(textureData);
}

/*
*	Creates and returns a uniform buffer.
*/
UniformBufferHandle RenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT
{
	//Create the uniform buffer via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Sets the active camera.
*/
void RenderingSystem::SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT
{
	//Set the active camera via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->SetActiveCamera(newActiveCamera);
}

/*
*	Sets the post processing blur amount.
*/
void RenderingSystem::SetPostProcessingBlurAmount(const float newBlurAmount) NOEXCEPT
{
	//Set the post processing blur amount via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->SetPostProcessingBlurAmount(newBlurAmount);
}

/*
*	Sets the post processing chromatic aberration amount.
*/
void RenderingSystem::SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) NOEXCEPT
{
	//Set the post processing chromatic aberration amount via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->SetPostProcessingChromaticAberrationAmount(newChromaticAberrationAmount);
}

/*
*	Sets the post processing saturation.
*/
void RenderingSystem::SetPostProcessingSaturation(const float newSaturation) NOEXCEPT
{
	//Set the post processing saturation via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->SetPostProcessingSaturation(newSaturation);
}

/*
*	Sets the post processing sharpen amount.
*/
void RenderingSystem::SetPostProcessingSharpenAmount(const float newSharpenAmount) NOEXCEPT
{
	//Set the post processing sharpen amount via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->SetPostProcessingSharpenAmount(newSharpenAmount);
}

//Undefine defines to keep them from leaking into other scopes.
#undef CURRENT_RENDERING_SYSTEM