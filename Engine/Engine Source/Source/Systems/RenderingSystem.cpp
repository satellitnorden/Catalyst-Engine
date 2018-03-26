//Header file.
#include <Systems/RenderingSystem.h>

//Rendering.
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Translation Layer/Vulkan/VulkanRenderingSystem.h>

//System definition.
DEFINE_SYSTEM(RenderingSystem);

/*
*	Default constructor.
*/
RenderingSystem::RenderingSystem() NOEXCEPT
{

}

/*
*	Initializes the rendering system.
*/
void RenderingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the Vulkan rendering system.
	VulkanRenderingSystem::Instance->InitializeSystem();
}

/*
*	Updates the rendering system synchronously.
*/
void RenderingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Update the Vulkan rendering system synchronously.
	VulkanRenderingSystem::Instance->UpdateSystemSynchronous();
}

/*
*	Releases the rendering system.
*/
void RenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release the Vulkan rendering system.
	VulkanRenderingSystem::Instance->ReleaseSystem();
}

/*
*	Creates an environment material.
*/
void RenderingSystem::CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT
{
	//Create the environment material via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->CreateEnvironmentMaterial(environmentMaterialData, environmentMaterial);
}

/*
*	Creates a terrain material.
*/
void RenderingSystem::CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Create the terrain material via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->CreateTerrainMaterial(terrainMaterialData, terrainMaterial);
}

/*
*	Creates a physical model.
*/
void RenderingSystem::CreatePhysicalModel(const PhysicalModelData &physicalModelData, PhysicalModel &physicalModel) const NOEXCEPT
{
	//Create the physical model via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->CreatePhysicalModel(physicalModelData, physicalModel);
}

/*
*	Creates a physical material.
*/
void RenderingSystem::CreatePhysicalMaterial(const PhysicalMaterialData &physicalMaterialData, PhysicalMaterial &physicalMaterial) const NOEXCEPT
{
	//Create the physical material via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->CreatePhysicalMaterial(physicalMaterialData, physicalMaterial);
}

/*
*	Creates a water material.
*/
void RenderingSystem::CreateWaterMaterial(const WaterMaterialData &waterMaterialData, WaterMaterial &waterMaterial) const NOEXCEPT
{
	//Create the water material via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->CreateWaterMaterial(waterMaterialData, waterMaterial);
}

/*
*	Initializes a terrain entity.
*/
void RenderingSystem::InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture4 &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT
{
	//Initialize the terrain entity via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->InitializeTerrainEntity(terrainEntity, terrainPlaneResolution, terrainProperties, terrainUniformData, layerWeightsTexture, terrainMaterial);
}

/*
*	Initializes a static physical entity.
*/
void RenderingSystem::InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT
{
	//Initialize the static physical entity via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->InitializeStaticPhysicalEntity(staticPhysicalEntity, model, position, rotation, scale);
}

/*
*	Initializes an instanced physical entity.
*/
void RenderingSystem::InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Initialize the instanced physical entity via the current rendering system.
	VulkanRenderingSystem::Instance->InitializeInstancedPhysicalEntity(entity, model, transformations);
}

/*
*	Initializes a water entity.
*/
void RenderingSystem::InitializeWaterEntity(const WaterEntity *const RESTRICT waterEntity, const WaterMaterial &waterMaterial, const WaterUniformData &waterUniformData) const NOEXCEPT
{
	//Initialize the water entity via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->InitializeWaterEntity(waterEntity, waterMaterial, waterUniformData);
}

/*
*	Creates and returns a 2D texture given the texture data.
*/
Texture2DHandle RenderingSystem::Create2DTexture(const TextureData &textureData) const NOEXCEPT
{
	//Create the 2D texture via the Vulkan rendering system.
	return VulkanRenderingSystem::Instance->Create2DTexture(textureData);
}

/*
*	Creates and returns a cube map texture.
*/
TextureCubeMapHandle RenderingSystem::CreateCubeMapTexture(const char *RESTRICT frontTexturePath, const char *RESTRICT backTexturePath, const char *RESTRICT upTexturePath, const char *RESTRICT downTexturePath, const char *RESTRICT rightTexturePath, const char *RESTRICT leftTexturePath) const NOEXCEPT
{
	//Create the cube map via the Vulkan rendering system.
	return VulkanRenderingSystem::Instance->CreateCubeMapTexture(frontTexturePath, backTexturePath, upTexturePath, downTexturePath, rightTexturePath, leftTexturePath);
}

/*
*	Creates and returns a uniform buffer.
*/
UniformBufferHandle RenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT
{
	//Create the uniform buffer via the Vulkan rendering system.
	return VulkanRenderingSystem::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Sets the active camera.
*/
void RenderingSystem::SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT
{
	//Set the active camera via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->SetActiveCamera(newActiveCamera);
}

/*
*	Sets the active sky box cube map texture.
*/
void RenderingSystem::SetActiveSkyBox(TextureCubeMapHandle newSkyBox) NOEXCEPT
{
	//Set the active sky box cube map texture via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->SetActiveSkyBox(newSkyBox);
}

/*
*	Sets the post processing blur amount.
*/
void RenderingSystem::SetPostProcessingBlurAmount(const float newBlurAmount) NOEXCEPT
{
	//Set the post processing blur amount via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->SetPostProcessingBlurAmount(newBlurAmount);
}

/*
*	Sets the post processing chromatic aberration amount.
*/
void RenderingSystem::SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) NOEXCEPT
{
	//Set the post processing chromatic aberration amount via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->SetPostProcessingChromaticAberrationAmount(newChromaticAberrationAmount);
}

/*
*	Sets the post processing saturation.
*/
void RenderingSystem::SetPostProcessingSaturation(const float newSaturation) NOEXCEPT
{
	//Set the post processing saturation via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->SetPostProcessingSaturation(newSaturation);
}

/*
*	Sets the post processing sharpen amount.
*/
void RenderingSystem::SetPostProcessingSharpenAmount(const float newSharpenAmount) NOEXCEPT
{
	//Set the post processing sharpen amount via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->SetPostProcessingSharpenAmount(newSharpenAmount);
}