//Header file.
#include <Systems/RenderingSystem.h>

//Rendering.
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Engine Layer/Resolution.h>
#include <Rendering/Engine Layer/Render Passes/RenderPasses.h>
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

	//Initialize all render passes.
	TerrainRenderPass::Instance->Initialize();
	StaticPhysicalRenderPass::Instance->Initialize();
	InstancedPhysicalRenderPass::Instance->Initialize();
	VegetationRenderPass::Instance->Initialize();
	LightingRenderPass::Instance->Initialize();
}

/*
*	Updates the rendering system synchronously.
*/
void RenderingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Pre-update the current rendering system synchronously.
	CURRENT_RENDERING_SYSTEM::Instance->PreUpdateSystemSynchronous();

	//Render all render passes.
	TerrainRenderPass::Instance->Render();
	StaticPhysicalRenderPass::Instance->Render();
	InstancedPhysicalRenderPass::Instance->Render();
	VegetationRenderPass::Instance->Render();
	LightingRenderPass::Instance->Render();

	//Post-update the current rendering system synchronously.
	CURRENT_RENDERING_SYSTEM::Instance->PostUpdateSystemSynchronous();
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
*	Returns the render resolution.
*/
Resolution RenderingSystem::GetRenderResolution() const NOEXCEPT
{
	//Return the render resolution via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetRenderResolution();
}

/*
*	Returns the current frame index.
*/
uint8 RenderingSystem::GetCurrentFrameIndex() const NOEXCEPT
{
	//Return the current frame index via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentFrameIndex();
}

/*
*	Finalizes the initialization of a render pass.
*/
void RenderingSystem::FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Finalize the initialization of this render pass via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->FinalizeRenderPassInitialization(renderPass);
}

/*
*	Returns the current dynamic uniform data descriptor set.
*/
DescriptorSetHandle RenderingSystem::GetCurrentDynamicUniformDataDescriptorSet() const NOEXCEPT
{
	//Return the current dynamic uniform data descriptor set via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentDynamicUniformDataDescriptorSet();
}

/*
*	Returns the current environment data descriptor set.
*/
DescriptorSetHandle RenderingSystem::GetCurrentEnvironmentDataDescriptorSet() const NOEXCEPT
{
	//Return the current environment data descriptor set via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentEnvironmentDataDescriptorSet();
}

/*
*	Returns the lighting descriptor set.
*/
DescriptorSetHandle RenderingSystem::GetLightingDescriptorSet() const NOEXCEPT
{
	//Return the lighting descriptor set via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetLightingDescriptorSet();
}

/*
*	Returns the current directional shadow event.
*/
EventHandle RenderingSystem::GetCurrentDirectionalShadowEvent() const NOEXCEPT
{
	//Return the current directional shadow event via the current rendering system.
	return CURRENT_RENDERING_SYSTEM::Instance->GetCurrentDirectionalShadowEvent();
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
void RenderingSystem::CreateWaterMaterial(const WaterMaterialData &waterMaterialData, OceanMaterial &oceanMaterial) const NOEXCEPT
{
	//Create the water material via the current rendering system.
	CURRENT_RENDERING_SYSTEM::Instance->CreateWaterMaterial(waterMaterialData, oceanMaterial);
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