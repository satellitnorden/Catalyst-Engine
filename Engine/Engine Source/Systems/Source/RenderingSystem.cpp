//Header file.
#include <RenderingSystem.h>

//Rendering.
#include <PhysicalModel.h>
#include <VulkanRenderingSystem.h>

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
*	Post-initializes the rendering system.
*/
void RenderingSystem::PostInitializeSystem() NOEXCEPT
{
	//Post-initialize the Vulkan rendering system.
	VulkanRenderingSystem::Instance->PostInitializeSystem();
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
*	Creates a terrain material.
*/
void RenderingSystem::CreateTerrainMaterial(const TerrainMaterialData &terrainMaterialData, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Create the terrain material via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->CreateTerrainMaterial(terrainMaterialData, terrainMaterial);
}

/*
*	Creates and returns physical model.
*/
const PhysicalModel RenderingSystem::CreatePhysicalModel(const char *RESTRICT modelPath, Texture2DHandle albedoTexture, Texture2DHandle normalMapTexture, Texture2DHandle roughnessTexture, Texture2DHandle metallicTexture, Texture2DHandle ambientOcclusionTexture) const NOEXCEPT
{
	//Create the physical model via the Vulkan rendering system.
	return VulkanRenderingSystem::Instance->CreatePhysicalModel(modelPath, albedoTexture, normalMapTexture, roughnessTexture, metallicTexture, ambientOcclusionTexture);
}

/*
*	Initializes a physical entity.
*/
void RenderingSystem::InitializePhysicalEntity(PhysicalEntity &physicalEntity, const PhysicalModel &model) const NOEXCEPT
{
	//Initialize the physical entity via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->InitializePhysicalEntity(physicalEntity, model);
}

/*
*	Initializes a terrain entity.
*/
void RenderingSystem::InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainPlaneResolution, const CPUTexture4 &terrainProperties, const TerrainUniformData &terrainUniformData, const Texture2DHandle layerWeightsTexture, const TerrainMaterial &terrainMaterial) const NOEXCEPT
{
	//Initialize the terrain entity via the Vulkan rendering system.
	VulkanRenderingSystem::Instance->InitializeTerrainEntity(terrainEntity,terrainPlaneResolution, terrainProperties, terrainUniformData, layerWeightsTexture, terrainMaterial);
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