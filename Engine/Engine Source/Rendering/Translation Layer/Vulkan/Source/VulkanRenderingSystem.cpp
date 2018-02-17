//Header file.
#include <VulkanRenderingSystem.h>

//Components.
#include <ComponentManager.h>

//Entities.
#include <CameraEntity.h>
#include <PhysicalEntity.h>
#include <PointLightEntity.h>
#include <SpotLightEntity.h>
#include <TerrainEntity.h>

//Math.
#include <GameMath.h>
#include <Matrix3.h>

//Rendering.
#include <CPUTexture4.h>
#include <ModelLoader.h>
#include <PhysicalModel.h>
#include <RenderingUtilities.h>
#include <ShaderLoader.h>
#include <TextureData.h>
#include <VulkanTranslationUtilities.h>

//Systems.
#include <EngineSystem.h>
#include <QuestSystem.h>

//Preprocessor defines
#define VULKAN_SHADERS_PATH "../../Engine Source/Rendering/Translation Layer/Vulkan/Shaders/"

//System definition.
DEFINE_SYSTEM(VulkanRenderingSystem);

/*
*	Default constructor.
*/
VulkanRenderingSystem::VulkanRenderingSystem() NOEXCEPT
{

}

/*
*	Initializes the Vulkan rendering system.
*/
void VulkanRenderingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the main window.
	mainWindow.Initialize();

	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize(mainWindow);

	//Initialize all render targets.
	InitializeRenderTargets();

	//Initialize all semaphores.
	InitializeSemaphores();

	//Initialize all uniform buffers.
	InitializeUniformBuffers();

	//Resize the number of command buffers to be equal to the amount of swapchain images.
	swapchainCommandBuffers.Resize(VulkanInterface::Instance->GetSwapchain().GetSwapChainImages().Size());

	//Allocate all command buffers.
	for (auto &commandBuffer : swapchainCommandBuffers)
	{
		VulkanInterface::Instance->GetGraphicsCommandPool().AllocateVulkanCommandBuffer(commandBuffer);
	}

	//Initialize all shader modules.
	InitializeShaderModules();

	//Initialize all pipelines.
	InitializePipelines();

	//Initialize all descriptor sets.
	InitializeDescriptorSets();

	//Initialize all default textures.
	InitializeDefaultTextures();
}

/*
*	Post-initializes the Vulkan rendering system.
*/
void VulkanRenderingSystem::PostInitializeSystem() NOEXCEPT
{
	//Register the graphics system update dynamic uniform data daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::RenderingSystemUpdateDynamicUniformData, [](void *RESTRICT arguments) { static_cast<VulkanRenderingSystem *RESTRICT>(arguments)->UpdateDynamicUniformData(); });

	//Register the graphics system update physical entities graphics buffers daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::RenderingSystemUpdatePhysicalEntitiesGraphicsBuffers, [](void *RESTRICT arguments) { static_cast<VulkanRenderingSystem *RESTRICT>(arguments)->UpdatePhysicalEntitiesGraphicsBuffers(); });

	//Register the graphics system update view frustum culling daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::RenderingSystemUpdateViewFrustumCulling, [](void *RESTRICT arguments) { static_cast<VulkanRenderingSystem *RESTRICT>(arguments)->UpdateViewFrustumCulling(); });
}

/*
*	Updates the graphics system synchronously.
*/
void VulkanRenderingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Carry out the graphics system update physical entities graphics buffers daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::RenderingSystemUpdatePhysicalEntitiesGraphicsBuffers, this);

	//Carry out the graphics system asynchronous update daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::RenderingSystemUpdateViewFrustumCulling, this);

	//Update the main window.
	mainWindow.Update();

	//Check if the main window should close - If so, terminate immediately.
	if (mainWindow.ShouldClose())
	{
		EngineSystem::Instance->Terminate();

		return;
	}

	//Pre-update the Vulkan interface.
	VulkanInterface::Instance->PreUpdate(semaphores[Semaphore::ImageAvailable]);

	//Begin the frame.
	BeginFrame();

	//Render the terrain.
	RenderTerrain();

	//Render all physical entities.
	RenderPhysicalEntities();

	//Render the lighting.
	RenderLighting();

	//Render the sky box.
	RenderSkyBox();

	//Render the post processing.
	RenderPostProcessing();

	//End the frame.
	EndFrame();

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(semaphores[Semaphore::RenderFinished]);
}

/*
*	Releases the Vulkan rendering system.
*/
void VulkanRenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release the main window.
	mainWindow.Release();

	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Creates and returns physical model.
*/
const PhysicalModel VulkanRenderingSystem::CreatePhysicalModel(const char *RESTRICT modelPath, Texture2DHandle albedoTexture, Texture2DHandle normalMapTexture, Texture2DHandle roughnessTexture, Texture2DHandle metallicTexture, Texture2DHandle ambientOcclusionTexture) const NOEXCEPT
{
	//Load the model.
	DynamicArray<Vertex> vertices;
	DynamicArray<uint32> indices;
	float extent{ 0.0f };

	ModelLoader::LoadModel(modelPath, vertices, indices, extent);

	//Create the vertex and index buffer.
	const void *RESTRICT modelData[]{ vertices.Data(), indices.Data() };
	const VkDeviceSize modelDataSizes[]{ sizeof(Vertex) * vertices.Size(), sizeof(uint32) * indices.Size() };
	VulkanBuffer *RESTRICT buffer = VulkanInterface::Instance->CreateBuffer(modelData, modelDataSizes, 2);

	//Set up the physical model.
	PhysicalModel newPhysicalModel;

	newPhysicalModel.GetAxisAlignedBoundingBox().minimum = Vector3(-extent, -extent, -extent);
	newPhysicalModel.GetAxisAlignedBoundingBox().maximum = Vector3(extent, extent, extent);
	newPhysicalModel.SetBuffer(buffer);
	newPhysicalModel.SetIndexOffset(modelDataSizes[0]);
	newPhysicalModel.GetMaterial().SetAlbedoTexture(albedoTexture);
	newPhysicalModel.GetMaterial().SetNormalMapTexture(normalMapTexture);
	newPhysicalModel.GetMaterial().SetRoughnessTexture(roughnessTexture ? roughnessTexture : defaultTextures[DefaultTexture::White]);
	newPhysicalModel.GetMaterial().SetMetallicTexture(metallicTexture ? metallicTexture : defaultTextures[DefaultTexture::Black]);
	newPhysicalModel.GetMaterial().SetAmbientOcclusionTexture(ambientOcclusionTexture ? ambientOcclusionTexture : defaultTextures[DefaultTexture::White]);
	newPhysicalModel.SetIndexCount(static_cast<uint32>(indices.Size()));

	return newPhysicalModel;
}

/*
*	Initializes a physical entity.
*/
void VulkanRenderingSystem::InitializePhysicalEntity(PhysicalEntity &physicalEntity, const PhysicalModel &model) const NOEXCEPT
{
	//Cache relevant data.
	VulkanDescriptorSet newDescriptorSet;
	VulkanUniformBuffer newUniformBuffer{ *static_cast<VulkanUniformBuffer *RESTRICT>(CreateUniformBuffer(sizeof(Matrix4))) };
	const PhysicalMaterial &material = model.GetMaterial();
	const Texture2DHandle albedoTexture = material.GetAlbedoTexture();
	const Texture2DHandle normalMapTexture = material.GetNormalMapTexture();
	const Texture2DHandle roughnessTexture = material.GetRoughnessTexture();
	const Texture2DHandle metallicTexture = material.GetMetallicTexture();
	const Texture2DHandle ambientOcclusionTexture = material.GetAmbientOcclusionTexture();

	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, pipelines[Pipeline::SceneBufferPipeline]->GetDescriptorSetLayout());

	//Update the write descriptor sets.
	DynamicArray<VkWriteDescriptorSet, 7> writeDescriptorSets;

	writeDescriptorSets.EmplaceFast(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(newDescriptorSet, 0));
	writeDescriptorSets.EmplaceFast(newUniformBuffer.GetWriteDescriptorSet(newDescriptorSet, 1));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 2));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(normalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 3));
	writeDescriptorSets.EmplaceFast(roughnessTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(roughnessTexture)->GetWriteDescriptorSet(newDescriptorSet, 4) : defaultTextures[DefaultTexture::White]->GetWriteDescriptorSet(newDescriptorSet, 4));
	writeDescriptorSets.EmplaceFast(metallicTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(metallicTexture)->GetWriteDescriptorSet(newDescriptorSet, 5) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(newDescriptorSet, 5));
	writeDescriptorSets.EmplaceFast(ambientOcclusionTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(ambientOcclusionTexture)->GetWriteDescriptorSet(newDescriptorSet, 6) : defaultTextures[DefaultTexture::White]->GetWriteDescriptorSet(newDescriptorSet, 6));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Fill the physical entity components with the relevant data.
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetPhysicalFrustumCullingComponents()[physicalEntity.GetComponentsIndex()] };
	GraphicsBufferComponent &graphicsBufferComponent{ ComponentManager::GetPhysicalGraphicsBufferComponents()[physicalEntity.GetComponentsIndex()] };
	RenderComponent &renderComponent{ ComponentManager::GetPhysicalRenderComponents()[physicalEntity.GetComponentsIndex()] };

	frustumCullingComponent.axisAlignedBoundingBox = model.GetAxisAlignedBoundingBox();
	graphicsBufferComponent.uniformBuffer = newUniformBuffer;
	renderComponent.descriptorSet = newDescriptorSet;
	renderComponent.buffer = *model.GetBuffer();
	renderComponent.indexOffset = model.GetIndexOffset();
	renderComponent.indexCount = model.GetIndexCount();
}

/*
*	Initializes a terrain entity.
*/
void VulkanRenderingSystem::InitializeTerrainEntity(TerrainEntity &terrainEntity, const CPUTexture4 &heightMap, const uint32 terrainPlaneResolution, const TerrainUniformData &terrainUniformData, const Texture2DHandle terrainHeightMapTexture, const Texture2DHandle terrainNormalMapTexture, const Texture2DHandle layer1WeightTexture, const Texture2DHandle layer1AlbedoTexture, const Texture2DHandle layer1NormalMapTexture, const Texture2DHandle layer1RoughnessTexture, const Texture2DHandle layer1MetallicTexture, const Texture2DHandle layer1AmbientOcclusionTexture, const Texture2DHandle layer1DisplacementTexture, const Texture2DHandle layer2WeightTexture, const Texture2DHandle layer2AlbedoTexture, const Texture2DHandle layer2NormalMapTexture, const Texture2DHandle layer2RoughnessTexture, const Texture2DHandle layer2MetallicTexture, const Texture2DHandle layer2AmbientOcclusionTexture, const Texture2DHandle layer2DisplacementTexture, const Texture2DHandle layer3WeightTexture, const Texture2DHandle layer3AlbedoTexture, const Texture2DHandle layer3NormalMapTexture, const Texture2DHandle layer3RoughnessTexture, const Texture2DHandle layer3MetallicTexture, const Texture2DHandle layer3AmbientOcclusionTexture, const Texture2DHandle layer3DisplacementTexture) const NOEXCEPT
{
	//Generate the plane vertices and indices.
	DynamicArray<float> terrainVertices;
	DynamicArray<uint32> terrainIndices;
	RenderingUtilities::GeneratePlane(terrainPlaneResolution, terrainVertices, terrainIndices);

	//Create the vertex and index buffer.
	const void *RESTRICT terrainData[]{ terrainVertices.Data(), terrainIndices.Data() };
	const VkDeviceSize terrainDataSizes[]{ sizeof(float) * terrainVertices.Size(), sizeof(uint32) * terrainIndices.Size() };
	const VulkanBuffer terrainVertexBuffer{ *VulkanInterface::Instance->CreateBuffer(terrainData, terrainDataSizes, 2) };

	//Fill the terrain entity components with the relevant data.
	TerrainComponent &terrainComponent{ ComponentManager::GetTerrainComponents()[terrainEntity.GetComponentsIndex()] };
	TerrainRenderComponent &terrainRenderComponent{ ComponentManager::GetTerrainRenderComponents()[terrainEntity.GetComponentsIndex()] };

	terrainComponent.terrainUniformData = terrainUniformData;
	terrainComponent.uniformBuffer = *VulkanInterface::Instance->CreateUniformBuffer(sizeof(TerrainUniformData));
	terrainComponent.uniformBuffer.UploadData(&terrainComponent.terrainUniformData);
	terrainComponent.heightMap = heightMap;

	//Create the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(terrainRenderComponent.descriptorSet, pipelines[Pipeline::TerrainSceneBufferPipeline]->GetDescriptorSetLayout());

	DynamicArray<VkWriteDescriptorSet, 25> writeDescriptorSets;

	writeDescriptorSets.EmplaceFast(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 0));
	writeDescriptorSets.EmplaceFast(terrainComponent.uniformBuffer.GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 1));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainHeightMapTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 2));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainNormalMapTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 3));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(layer1WeightTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 4));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(layer1AlbedoTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 5));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(layer1NormalMapTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 6));
	writeDescriptorSets.EmplaceFast(layer1RoughnessTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer1RoughnessTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 7) : defaultTextures[DefaultTexture::White]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 7));
	writeDescriptorSets.EmplaceFast(layer1MetallicTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer1MetallicTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 8) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 8));
	writeDescriptorSets.EmplaceFast(layer1AmbientOcclusionTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer1AmbientOcclusionTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 9) : defaultTextures[DefaultTexture::White]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 9));
	writeDescriptorSets.EmplaceFast(layer1DisplacementTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer1DisplacementTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 10) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 10));
	writeDescriptorSets.EmplaceFast(layer2WeightTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer2WeightTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 11) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 11));
	writeDescriptorSets.EmplaceFast(layer2AlbedoTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer2AlbedoTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 12) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 12));
	writeDescriptorSets.EmplaceFast(layer2NormalMapTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer2NormalMapTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 13) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 13));
	writeDescriptorSets.EmplaceFast(layer2RoughnessTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer2RoughnessTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 14) : defaultTextures[DefaultTexture::White]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 14));
	writeDescriptorSets.EmplaceFast(layer2MetallicTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer2MetallicTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 15) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 15));
	writeDescriptorSets.EmplaceFast(layer2AmbientOcclusionTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer2AmbientOcclusionTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 16) : defaultTextures[DefaultTexture::White]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 16));
	writeDescriptorSets.EmplaceFast(layer2DisplacementTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer2DisplacementTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 17) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 17));
	writeDescriptorSets.EmplaceFast(layer3WeightTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer3WeightTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 18) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 18));
	writeDescriptorSets.EmplaceFast(layer3AlbedoTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer3AlbedoTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 19) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 19));
	writeDescriptorSets.EmplaceFast(layer3NormalMapTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer3NormalMapTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 20) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 20));
	writeDescriptorSets.EmplaceFast(layer3RoughnessTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer3RoughnessTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 21) : defaultTextures[DefaultTexture::White]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 21));
	writeDescriptorSets.EmplaceFast(layer3MetallicTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer3MetallicTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 22) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 22));
	writeDescriptorSets.EmplaceFast(layer3AmbientOcclusionTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer3AmbientOcclusionTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 23) : defaultTextures[DefaultTexture::White]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 23));
	writeDescriptorSets.EmplaceFast(layer3DisplacementTexture ? static_cast<const Vulkan2DTexture *RESTRICT>(layer3DisplacementTexture)->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 24) : defaultTextures[DefaultTexture::Black]->GetWriteDescriptorSet(terrainRenderComponent.descriptorSet, 24));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	terrainRenderComponent.vertexAndIndexBuffer = terrainVertexBuffer;
	terrainRenderComponent.indexBufferOffset = static_cast<uint32>(sizeof(float) * terrainVertices.Size());
	terrainRenderComponent.indexCount = static_cast<uint32>(terrainIndices.Size());
}

/*
*	Creates and returns a 2D texture given the texture data.
*/
Texture2DHandle VulkanRenderingSystem::Create2DTexture(const TextureData &textureData) const NOEXCEPT
{
	//Create the Vulkan 2D texture.
	Vulkan2DTexture *RESTRICT new2DTexture = VulkanInterface::Instance->Create2DTexture(static_cast<uint32>(textureData.textureDataContainer.textureData.Size()), textureData.textureDataContainer.textureWidth, textureData.textureDataContainer.textureHeight, textureData.textureDataContainer.textureChannels, textureData.textureDataContainer.textureTexelSize, textureData.textureDataContainer.textureData.Data(), VulkanTranslationUtilities::GetVulkanFormat(textureData.textureFormat), VulkanTranslationUtilities::GetVulkanTextureFilter(textureData.magnificationFilter), VulkanTranslationUtilities::GetVulkanMipmapMode(textureData.mipmapMode), VulkanTranslationUtilities::GetVulkanAddressMode(textureData.addressMode));

	//Return the texture.
	return static_cast<Texture2DHandle>(new2DTexture);
}

/*
*	Creates and returns a cube map texture.
*/
TextureCubeMapHandle VulkanRenderingSystem::CreateCubeMapTexture(const char *RESTRICT frontTexturePath, const char *RESTRICT backTexturePath, const char *RESTRICT upTexturePath, const char *RESTRICT downTexturePath, const char *RESTRICT rightTexturePath, const char *RESTRICT leftTexturePath) const NOEXCEPT
{
	//Load all textures.
	byte *RESTRICT textureData[6];

	uint16 width = 0;
	uint16 height = 0;
	uint8 numberOfChannels = 0;

	TextureLoader::LoadTexture(frontTexturePath, width, height, numberOfChannels, &textureData[0]);
	TextureLoader::LoadTexture(backTexturePath, width, height, numberOfChannels, &textureData[1]);
	TextureLoader::LoadTexture(upTexturePath, width, height, numberOfChannels, &textureData[2]);
	TextureLoader::LoadTexture(downTexturePath, width, height, numberOfChannels, &textureData[3]);
	TextureLoader::LoadTexture(rightTexturePath, width, height, numberOfChannels, &textureData[4]);
	TextureLoader::LoadTexture(leftTexturePath, width, height, numberOfChannels, &textureData[5]);

	//Create the Vulkan 2D texture.
	VulkanCubeMapTexture *RESTRICT newCubeMapTexture = VulkanInterface::Instance->CreateCubeMapTexture(static_cast<uint32>(width), static_cast<uint32>(height), reinterpret_cast<const byte *RESTRICT *RESTRICT>(&textureData));

	//Free the texture.
	TextureLoader::FreeTexture(textureData[0]);
	TextureLoader::FreeTexture(textureData[1]);
	TextureLoader::FreeTexture(textureData[2]);
	TextureLoader::FreeTexture(textureData[3]);
	TextureLoader::FreeTexture(textureData[4]);
	TextureLoader::FreeTexture(textureData[5]);

	//Return the texture.
	return static_cast<TextureCubeMapHandle>(newCubeMapTexture);
}

/*
*	Creates a uniform buffer and returns the identifier for the uniform buffer.
*/
UniformBufferHandle VulkanRenderingSystem::CreateUniformBuffer(const size_t uniformBufferSize) const NOEXCEPT
{
	return VulkanInterface::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Sets the active camera.
*/
void VulkanRenderingSystem::SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT
{
	//Set the active camera.
	activeCamera = newActiveCamera;

	//Calculate the projection matrix.
	CalculateProjectionMatrix();
}

/*
*	Sets the active sky box cube map texture.
*/
void VulkanRenderingSystem::SetActiveSkyBox(TextureCubeMapHandle newSkyBox) NOEXCEPT
{
	//Update the sky box texture.
	skyBoxTexture = static_cast<VulkanCubeMapTexture *RESTRICT>(newSkyBox);

	//Allocate the sky box descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(skyBoxDescriptorSet, pipelines[Pipeline::CubeMapPipeline]->GetDescriptorSetLayout());

	//Update the write descriptor sets.
	DynamicArray<VkWriteDescriptorSet, 2> writeDescriptorSets;

	writeDescriptorSets.EmplaceFast(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(skyBoxDescriptorSet, 0));
	writeDescriptorSets.EmplaceFast(skyBoxTexture->GetWriteDescriptorSet(skyBoxDescriptorSet, 1));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Reinitialize the descriptor sets.
	ReinitializeDescriptorSets();
}

/*
*	Sets the post processing blur amount.
*/
void VulkanRenderingSystem::SetPostProcessingBlurAmount(const float newBlurAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.blurAmount = newBlurAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing chromatic aberration amount.
*/
void VulkanRenderingSystem::SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.chromaticAberrationAmount = newChromaticAberrationAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing saturation.
*/
void VulkanRenderingSystem::SetPostProcessingSaturation(const float newSaturation) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.saturation = newSaturation;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing sharpen amount.
*/
void VulkanRenderingSystem::SetPostProcessingSharpenAmount(const float newSharpenAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.sharpenAmount = newSharpenAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Initializes all render targets.
*/
void VulkanRenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Initialize all depth buffers.
	depthBuffers[DepthBuffer::SceneBufferDepthBuffer] = VulkanInterface::Instance->CreateDepthBuffer(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Initialize all render targets.
	renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[RenderTarget::SceneRenderTarget] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
}

/*
*	Initializes all semaphores.
*/
void VulkanRenderingSystem::InitializeSemaphores() NOEXCEPT
{
	//Initialize all semaphores.
	semaphores[Semaphore::ImageAvailable] = VulkanInterface::Instance->CreateSemaphore();
	semaphores[Semaphore::RenderFinished] = VulkanInterface::Instance->CreateSemaphore();
}

/*
*	Initializes all uniform buffers.
*/
void VulkanRenderingSystem::InitializeUniformBuffers() NOEXCEPT
{
	//Create the dynamic uniform data buffer.
	uniformBuffers[UniformBuffer::DynamicUniformDataBuffer] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(VulkanDynamicUniformData));

	//Create the post processing uniform data buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(PostProcessingUniformData));

	//Upload the initial data to the post processing uniform data buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Initializes all shader modules.
*/
void VulkanRenderingSystem::InitializeShaderModules() NOEXCEPT
{
	//Initialize the cube map fragment shader module.
	const auto cubeMapFragmentShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "CubeMapFragmentShader.spv");
	shaderModules[ShaderModule::CubeMapFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(cubeMapFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the cube map vertex shader module.
	const auto cubeMapVertexShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "CubeMapVertexShader.spv");
	shaderModules[ShaderModule::CubeMapVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(cubeMapVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the lighting fragment shader module.
	const auto lightingFragmentShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "LightingFragmentShader.spv");
	shaderModules[ShaderModule::LightingFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(lightingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the post processing fragment shader module.
	const auto postProcessingFragmentShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "PostProcessingFragmentShader.spv");
	shaderModules[ShaderModule::PostProcessingFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(postProcessingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the scene buffer fragment shader module.
	const auto sceneBufferFragmentShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "SceneBufferFragmentShader.spv");
	shaderModules[ShaderModule::SceneBufferFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(sceneBufferFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the scene buffer vertex shader module.
	const auto sceneBufferVertexShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "SceneBufferVertexShader.spv");
	shaderModules[ShaderModule::SceneBufferVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(sceneBufferVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the terrain scene buffer fragment shader module.
	const auto terrainSceneBufferFragmentShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "TerrainSceneBufferFragmentShader.spv");
	shaderModules[ShaderModule::TerrainSceneBufferFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the terrain scene buffer tessellation control shader module.
	const auto terrainSceneBufferTessellationControlShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "TerrainSceneBufferTessellationControlShader.spv");
	shaderModules[ShaderModule::TerrainSceneBufferTessellationControlShaderModule] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferTessellationControlShaderByteCode, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);

	//Initialize the terrain scene buffer tessellation evaluation shader module.
	const auto terrainSceneBufferTessellationEvaluationShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "TerrainSceneBufferTessellationEvaluationShader.spv");
	shaderModules[ShaderModule::TerrainSceneBufferTessellationEvaluationShaderModule] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferTessellationEvaluationShaderByteCode, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

	//Initialize the terrain scene buffer vertex shader module.
	const auto terrainSceneBufferVertexShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "TerrainSceneBufferVertexShader.spv");
	shaderModules[ShaderModule::TerrainSceneBufferVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(terrainSceneBufferVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the lighting vertex shader module.
	const auto viewportVertexShaderByteCode = ShaderLoader::LoadShader(VULKAN_SHADERS_PATH "ViewportVertexShader.spv");
	shaderModules[ShaderModule::ViewportVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(viewportVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
}

/*
*	Initializes all pipelines.
*/
void VulkanRenderingSystem::InitializePipelines() NOEXCEPT
{
	//Create the terrain scene buffer pipeline.
	VulkanPipelineCreationParameters terrainSceneBufferPipelineCreationParameters;

	terrainSceneBufferPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
	terrainSceneBufferPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	terrainSceneBufferPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	terrainSceneBufferPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	terrainSceneBufferPipelineCreationParameters.colorAttachments.Resize(1);
	terrainSceneBufferPipelineCreationParameters.colorAttachments[0].Reserve(3);
	terrainSceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetImageView());
	terrainSceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget]->GetImageView());
	terrainSceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget]->GetImageView());
	terrainSceneBufferPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	terrainSceneBufferPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	terrainSceneBufferPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	terrainSceneBufferPipelineCreationParameters.depthBuffers.Reserve(1);
	terrainSceneBufferPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
	terrainSceneBufferPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
	terrainSceneBufferPipelineCreationParameters.depthTestEnable = VK_TRUE;
	terrainSceneBufferPipelineCreationParameters.depthWriteEnable = VK_TRUE;
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(25);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(11, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(12, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(13, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(14, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(15, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(16, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(17, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(18, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(19, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(20, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(21, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(22, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(23, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	terrainSceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(24, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	terrainSceneBufferPipelineCreationParameters.shaderModules.Reserve(4);
	terrainSceneBufferPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::TerrainSceneBufferVertexShaderModule]);
	terrainSceneBufferPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::TerrainSceneBufferTessellationControlShaderModule]);
	terrainSceneBufferPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::TerrainSceneBufferTessellationEvaluationShaderModule]);
	terrainSceneBufferPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::TerrainSceneBufferFragmentShaderModule]);
	terrainSceneBufferPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	VulkanTranslationUtilities::GetTerrainVertexInputAttributeDescriptions(terrainSceneBufferPipelineCreationParameters.vertexInputAttributeDescriptions);
	VulkanTranslationUtilities::GetTerrainVertexInputBindingDescription(terrainSceneBufferPipelineCreationParameters.vertexInputBindingDescription);
	terrainSceneBufferPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[Pipeline::TerrainSceneBufferPipeline] = VulkanInterface::Instance->CreatePipeline(terrainSceneBufferPipelineCreationParameters);

	//Create the scene buffer pipeline.
	VulkanPipelineCreationParameters sceneBufferPipelineCreationParameters;

	sceneBufferPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
	sceneBufferPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	sceneBufferPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	sceneBufferPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	sceneBufferPipelineCreationParameters.colorAttachments.Resize(1);
	sceneBufferPipelineCreationParameters.colorAttachments[0].Reserve(3);
	sceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetImageView());
	sceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget]->GetImageView());
	sceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget]->GetImageView());
	sceneBufferPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	sceneBufferPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	sceneBufferPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	sceneBufferPipelineCreationParameters.depthBuffers.Reserve(1);
	sceneBufferPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
	sceneBufferPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
	sceneBufferPipelineCreationParameters.depthTestEnable = VK_TRUE;
	sceneBufferPipelineCreationParameters.depthWriteEnable = VK_TRUE;
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(7);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.shaderModules.Reserve(2);
	sceneBufferPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::SceneBufferVertexShaderModule]);
	sceneBufferPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::SceneBufferFragmentShaderModule]);
	sceneBufferPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	VulkanTranslationUtilities::GetPhysicalVertexInputAttributeDescriptions(sceneBufferPipelineCreationParameters.vertexInputAttributeDescriptions);
	VulkanTranslationUtilities::GetPhysicalVertexInputBindingDescription(sceneBufferPipelineCreationParameters.vertexInputBindingDescription);
	sceneBufferPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[Pipeline::SceneBufferPipeline] = VulkanInterface::Instance->CreatePipeline(sceneBufferPipelineCreationParameters);

	//Create the physical pipeline.
	VulkanPipelineCreationParameters lightingPipelineCreationParameters;

	lightingPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
	lightingPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	lightingPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.colorAttachments.Resize(1);
	lightingPipelineCreationParameters.colorAttachments[0].Reserve(1);
	lightingPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[RenderTarget::SceneRenderTarget]->GetImageView());
	lightingPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	lightingPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
	lightingPipelineCreationParameters.depthTestEnable = VK_FALSE;
	lightingPipelineCreationParameters.depthWriteEnable = VK_FALSE;
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(7);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.shaderModules.Reserve(2);
	lightingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::ViewportVertexShaderModule]);
	lightingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::LightingFragmentShaderModule]);
	lightingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	VulkanTranslationUtilities::GetDefaultVertexInputBindingDescription(lightingPipelineCreationParameters.vertexInputBindingDescription);
	lightingPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[Pipeline::LightingPipeline] = VulkanInterface::Instance->CreatePipeline(lightingPipelineCreationParameters);

	//Create the cube map pipeline.
	VulkanPipelineCreationParameters cubeMapPipelineCreationParameters;

	cubeMapPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
	cubeMapPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	cubeMapPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	cubeMapPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	cubeMapPipelineCreationParameters.colorAttachments.Resize(1);
	cubeMapPipelineCreationParameters.colorAttachments[0].Reserve(1);
	cubeMapPipelineCreationParameters.colorAttachments[0].EmplaceFast(renderTargets[RenderTarget::SceneRenderTarget]->GetImageView());
	cubeMapPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	cubeMapPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	cubeMapPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	cubeMapPipelineCreationParameters.depthBuffers.Reserve(1);
	cubeMapPipelineCreationParameters.depthBuffers.EmplaceFast(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
	cubeMapPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	cubeMapPipelineCreationParameters.depthTestEnable = VK_TRUE;
	cubeMapPipelineCreationParameters.depthWriteEnable = VK_TRUE;
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(2);
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	cubeMapPipelineCreationParameters.shaderModules.Reserve(2);
	cubeMapPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::CubeMapVertexShaderModule]);
	cubeMapPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::CubeMapFragmentShaderModule]);
	cubeMapPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	VulkanTranslationUtilities::GetDefaultVertexInputBindingDescription(cubeMapPipelineCreationParameters.vertexInputBindingDescription);
	cubeMapPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[Pipeline::CubeMapPipeline] = VulkanInterface::Instance->CreatePipeline(cubeMapPipelineCreationParameters);

	//Create the post processing pipeline.
	VulkanPipelineCreationParameters postProcessingPipelineCreationParameters;

	postProcessingPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
	postProcessingPipelineCreationParameters.colorAttachmentFinalLayout = VULKAN_IMAGE_LAYOUT_PRESENT_SRC;
	postProcessingPipelineCreationParameters.colorAttachmentFormat = VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format;
	postProcessingPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	const DynamicArray<VkImageView> &swapchainImageViews{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
	const size_t swapchainImageViewsSize{ swapchainImageViews.Size() };

	postProcessingPipelineCreationParameters.colorAttachments.Resize(swapchainImageViewsSize);

	for (size_t i = 0; i < swapchainImageViewsSize; ++i)
	{
		postProcessingPipelineCreationParameters.colorAttachments[i].Reserve(1);
		postProcessingPipelineCreationParameters.colorAttachments[i].EmplaceFast(swapchainImageViews[i]);
	}

	postProcessingPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	postProcessingPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	postProcessingPipelineCreationParameters.depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	postProcessingPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
	postProcessingPipelineCreationParameters.depthTestEnable = VK_FALSE;
	postProcessingPipelineCreationParameters.depthWriteEnable = VK_FALSE;
	postProcessingPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(3);
	postProcessingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	postProcessingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	postProcessingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceFast(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	postProcessingPipelineCreationParameters.shaderModules.Reserve(2);
	postProcessingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::ViewportVertexShaderModule]);
	postProcessingPipelineCreationParameters.shaderModules.EmplaceFast(shaderModules[ShaderModule::PostProcessingFragmentShaderModule]);
	postProcessingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	VulkanTranslationUtilities::GetDefaultVertexInputBindingDescription(postProcessingPipelineCreationParameters.vertexInputBindingDescription);
	postProcessingPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[Pipeline::PostProcessingPipeline] = VulkanInterface::Instance->CreatePipeline(postProcessingPipelineCreationParameters);
}

/*
*	Initializes all descriptor sets.
*/
void VulkanRenderingSystem::InitializeDescriptorSets() NOEXCEPT
{
	{
		//Initialize the lighting descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], pipelines[Pipeline::LightingPipeline]->GetDescriptorSetLayout());

		//Update the write descriptor sets.
		DynamicArray<VkWriteDescriptorSet, 5> writeDescriptorSets;

		writeDescriptorSets.EmplaceFast(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 0));
		writeDescriptorSets.EmplaceFast(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 1));
		writeDescriptorSets.EmplaceFast(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 2));
		writeDescriptorSets.EmplaceFast(renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 3));
		writeDescriptorSets.EmplaceFast(renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 4));

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the post processing descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], pipelines[Pipeline::PostProcessingPipeline]->GetDescriptorSetLayout());

		//Update the write descriptor sets.
		DynamicArray<VkWriteDescriptorSet, 3> writeDescriptorSets;

		writeDescriptorSets.EmplaceFast(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], 0));
		writeDescriptorSets.EmplaceFast(uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], 1));
		writeDescriptorSets.EmplaceFast(renderTargets[RenderTarget::SceneRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], 2));

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}
}

/*
*	Initializes all default textures.
*/
void VulkanRenderingSystem::InitializeDefaultTextures() NOEXCEPT
{
	byte defaultMetallic[]{ 0 };
	defaultTextures[DefaultTexture::Black] = static_cast<Vulkan2DTexture *RESTRICT>(Create2DTexture(TextureData(TextureDataContainer(defaultMetallic, 1, 1, 1), AddressMode::Repeat, TextureFilter::Nearest, MipmapMode::Nearest, TextureFormat::R8_Byte)));

	byte defaultRoughness[]{ 255 };
	defaultTextures[DefaultTexture::White] = static_cast<Vulkan2DTexture *RESTRICT>(Create2DTexture(TextureData(TextureDataContainer(defaultRoughness, 1, 1, 1), AddressMode::Repeat, TextureFilter::Nearest, MipmapMode::Nearest, TextureFormat::R8_Byte)));
}

/*
*	Calculates the projection matrix.
*/
void VulkanRenderingSystem::CalculateProjectionMatrix() NOEXCEPT
{
	//Calculate the projection matrix.
	projectionMatrix = Matrix4::Perspective(GameMath::DegreesToRadians(activeCamera->GetFieldOfView()), 1920.0f / 1080.0f, activeCamera->GetNearPlane(), activeCamera->GetFarPlane());
}

/*
*	Begins the frame.
*/
void VulkanRenderingSystem::BeginFrame() NOEXCEPT
{
	//Wait for the graphics queue to finish.
	VulkanInterface::Instance->GetGraphicsQueue().WaitIdle();

	//Carry out the graphics system update dynamic uniform data daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::RenderingSystemUpdateDynamicUniformData, this);

	//Set the current command buffer.
	currentSwapchainCommandBuffer = VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();

	//Set up the current command buffer.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}

/*
*	Renders the terrain.
*/
void VulkanRenderingSystem::RenderTerrain() NOEXCEPT
{
	//Cache the pipeline.
	VulkanPipeline &terrainSceneBufferPipeline{ *pipelines[Pipeline::TerrainSceneBufferPipeline] };
	VulkanCommandBuffer &commandBuffer{ swapchainCommandBuffers[currentSwapchainCommandBuffer] };

	//Begin the pipeline and render pass.
	commandBuffer.CommandBindPipeline(terrainSceneBufferPipeline);
	commandBuffer.CommandBeginRenderPass(terrainSceneBufferPipeline.GetRenderPass(), 0, true, 4);

	//Iterate over all terrain entity components and draw them all.
	const size_t numberOfTerrainEntityComponents{ ComponentManager::GetNumberOfTerrainComponents() };
	const TerrainRenderComponent *RESTRICT terrainRenderComponent{ ComponentManager::GetTerrainRenderComponents() };

	for (size_t i = 0; i < numberOfTerrainEntityComponents; ++i, ++terrainRenderComponent)
	{
		commandBuffer.CommandBindDescriptorSets(terrainSceneBufferPipeline, terrainRenderComponent->descriptorSet);
		commandBuffer.CommandBindVertexBuffers(terrainRenderComponent->vertexAndIndexBuffer);
		commandBuffer.CommandBindIndexBuffer(terrainRenderComponent->vertexAndIndexBuffer, terrainRenderComponent->indexBufferOffset);
		commandBuffer.CommandDrawIndexed(terrainRenderComponent->indexCount);
	}

	//End the render pass.
	commandBuffer.CommandEndRenderPass();
}

/*
*	Renders all physical entities.
*/
void VulkanRenderingSystem::RenderPhysicalEntities() NOEXCEPT
{
	//Wait for the physical entity update daily group quest to complete.
	QuestSystem::Instance->WaitForDailyQuest(DailyQuests::RenderingSystemUpdateViewFrustumCulling);

	//Cache the pipeline.
	VulkanPipeline &sceneBufferPipeline{ *pipelines[Pipeline::SceneBufferPipeline] };
	VulkanCommandBuffer &commandBuffer{ swapchainCommandBuffers[currentSwapchainCommandBuffer] };

	//Iterate over all physical entity components and draw them all.
	const size_t numberOfPhysicalEntityComponents{ ComponentManager::GetNumberOfPhysicalComponents() };
	const FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetPhysicalFrustumCullingComponents() };
	const RenderComponent *RESTRICT renderComponent{ ComponentManager::GetPhysicalRenderComponents() };

	if (numberOfPhysicalEntityComponents == 0)
	{
		return;
	}

	//Begin the pipeline and render pass.
	commandBuffer.CommandBindPipeline(sceneBufferPipeline);
	commandBuffer.CommandBeginRenderPass(sceneBufferPipeline.GetRenderPass(), 0, false, 0);

	for (size_t i = 0; i < numberOfPhysicalEntityComponents; ++i, ++frustumCullingComponent, ++renderComponent)
	{
		//Don't draw this physical entity if it isn't in the view frustum.
		if (!frustumCullingComponent->isInViewFrustum)
			continue;

		commandBuffer.CommandBindDescriptorSets(sceneBufferPipeline, renderComponent->descriptorSet);
		commandBuffer.CommandBindVertexBuffers(renderComponent->buffer);
		commandBuffer.CommandBindIndexBuffer(renderComponent->buffer, renderComponent->indexOffset);
		commandBuffer.CommandDrawIndexed(renderComponent->indexCount);
	}

	//End the render pass.
	commandBuffer.CommandEndRenderPass();
}

/*
*	Renders lighting.
*/
void VulkanRenderingSystem::RenderLighting() NOEXCEPT
{
	//Bind the lighting pipeline.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBindPipeline(*pipelines[Pipeline::LightingPipeline]);

	//Bind the lighting render pass.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBeginRenderPass(pipelines[Pipeline::LightingPipeline]->GetRenderPass(), 0, false, 1);

	//Bind the scene buffer descriptor set.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBindDescriptorSets(*pipelines[Pipeline::LightingPipeline], descriptorSets[DescriptorSet::LightingDescriptorSet]);

	//Draw the viewport!
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandDraw(4);

	//End the render pass.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandEndRenderPass();
}

/*
*	Renders sky box.
*/
void VulkanRenderingSystem::RenderSkyBox() NOEXCEPT
{
	//Bind the cube map pipeline.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBindPipeline(*pipelines[Pipeline::CubeMapPipeline]);

	//Bind the cube map render pass.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBeginRenderPass(pipelines[Pipeline::CubeMapPipeline]->GetRenderPass(), 0, false, 0);

	//Bind the sky box descriptor set.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBindDescriptorSets(*pipelines[Pipeline::CubeMapPipeline], skyBoxDescriptorSet);

	//Draw the sky box!
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandDraw(36);

	//End the cube map render pass.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandEndRenderPass();
}

/*
*	Renders the post processing.
*/
void VulkanRenderingSystem::RenderPostProcessing() NOEXCEPT
{
	//Bind the post processing pipeline.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBindPipeline(*pipelines[Pipeline::PostProcessingPipeline]);

	//Bind the post processing render pass.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBeginRenderPass(pipelines[Pipeline::PostProcessingPipeline]->GetRenderPass(), currentSwapchainCommandBuffer, false, 1);

	//Bind the post processing descriptor set.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBindDescriptorSets(*pipelines[Pipeline::PostProcessingPipeline], descriptorSets[DescriptorSet::PostProcessingDescriptorSet]);

	//Draw the viewport!
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandDraw(4);

	//End the post processing render pass.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandEndRenderPass();
}

/*
*	Ends the frame.
*/
void VulkanRenderingSystem::EndFrame() NOEXCEPT
{
	//Set up the proper parameters.
	static const DynamicArray<VkSemaphore> waitSemaphores{ semaphores[Semaphore::ImageAvailable]->Get() };
	static const VkPipelineStageFlags waitStages{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	static const DynamicArray<VkSemaphore> signalSemaphores{ semaphores[Semaphore::RenderFinished]->Get() };

	//End the current command buffer.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].End();

	//Wait for the graphics system update dynamic uniform data daily quest to finish.
	QuestSystem::Instance->WaitForDailyQuest(DailyQuests::RenderingSystemUpdateDynamicUniformData);

	//Wait for the graphics system update physical entities graphics buffers daily quest to finish.
	QuestSystem::Instance->WaitForDailyQuest(DailyQuests::RenderingSystemUpdatePhysicalEntitiesGraphicsBuffers);

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(swapchainCommandBuffers[currentSwapchainCommandBuffer], waitSemaphores, waitStages, signalSemaphores);
}

/*
*	Re-initializes all descriptor sets.
*/
void VulkanRenderingSystem::ReinitializeDescriptorSets() NOEXCEPT
{
	//Update the write descriptor sets.
	DynamicArray<VkWriteDescriptorSet, 5> writeDescriptorSets;

	writeDescriptorSets.EmplaceFast(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 0));
	writeDescriptorSets.EmplaceFast(skyBoxTexture->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 1));
	writeDescriptorSets.EmplaceFast(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 2));
	writeDescriptorSets.EmplaceFast(renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 3));
	writeDescriptorSets.EmplaceFast(renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 4));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
}

/*
*	Updates the dynamic uniform data.
*/
void VulkanRenderingSystem::UpdateDynamicUniformData() NOEXCEPT
{
	//Calculate the camera data
	Vector3 cameraWorldPosition = activeCamera->GetPosition();
	Vector3 forwardVector = activeCamera->GetForwardVector();
	Vector3 upVector = activeCamera->GetUpVector();

	Matrix4 cameraMatrix = Matrix4::LookAt(cameraWorldPosition, cameraWorldPosition + forwardVector, upVector);
	Matrix4 viewMatrix{ projectionMatrix * cameraMatrix };

	Matrix4 inverseCameraMatrix{ cameraMatrix };
	inverseCameraMatrix.Inverse();

	Matrix4 cameraOriginMatrix{ cameraMatrix };
	cameraOriginMatrix.SetTranslation(Vector3(0.0f, 0.0f, 0.0f));

	Matrix4 inverseProjectionMatrix{ projectionMatrix };
	inverseProjectionMatrix.Inverse();

	dynamicUniformData.cameraFieldOfViewCosine = GameMath::CosineDegrees(activeCamera->GetFieldOfView()) - 0.2f;
	dynamicUniformData.inverseCameraMatrix = inverseCameraMatrix;
	dynamicUniformData.inverseProjectionMatrix = inverseProjectionMatrix;
	dynamicUniformData.originViewMatrix = projectionMatrix * cameraOriginMatrix;
	dynamicUniformData.viewMatrix = viewMatrix;
	dynamicUniformData.cameraForwardVector = forwardVector;
	dynamicUniformData.cameraWorldPosition = cameraWorldPosition;

	const size_t numberOfDirectionalLightEntityComponents{ ComponentManager::GetNumberOfDirectionalLightComponents() };

	if (numberOfDirectionalLightEntityComponents > 0)
	{
		const DirectionalLightComponent *RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightComponents() };

		dynamicUniformData.directionalLightIntensity = directionalLightComponent->intensity;
		dynamicUniformData.directionalLightDirection = Vector3(0.0f, 0.0f, -1.0f).Rotated(directionalLightComponent->rotation);
		dynamicUniformData.directionalLightColor = directionalLightComponent->color;
		dynamicUniformData.directionalLightScreenSpacePosition = viewMatrix * Vector4(-dynamicUniformData.directionalLightDirection.X * 100.0f + cameraWorldPosition.X, -dynamicUniformData.directionalLightDirection.Y * 100.0f + cameraWorldPosition.Y, -dynamicUniformData.directionalLightDirection.Z * 100.0f + cameraWorldPosition.Z, 1.0f);
		dynamicUniformData.directionalLightScreenSpacePosition.X /= dynamicUniformData.directionalLightScreenSpacePosition.W;
		dynamicUniformData.directionalLightScreenSpacePosition.Y /= dynamicUniformData.directionalLightScreenSpacePosition.W;
		dynamicUniformData.directionalLightScreenSpacePosition.Z /= dynamicUniformData.directionalLightScreenSpacePosition.W;
	}

	else
	{
		dynamicUniformData.directionalLightIntensity = 0.0f;
		dynamicUniformData.directionalLightDirection = Vector3(0.0f, 0.0f, 0.0f);
		dynamicUniformData.directionalLightColor = Vector3(0.0f, 0.0f, 0.0f);
		dynamicUniformData.directionalLightScreenSpacePosition = Vector3(0.0f, 0.0f, 0.0f);
	}

	size_t counter = 0;

	const size_t numberOfPointLightEntityComponents{ ComponentManager::GetNumberOfPointLightComponents() };
	const PointLightComponent *RESTRICT pointLightComponent{ ComponentManager::GetPointLightComponents() };

	dynamicUniformData.numberOfPointLights = numberOfPointLightEntityComponents;

	for (size_t i = 0; i < numberOfPointLightEntityComponents; ++i, ++pointLightComponent)
	{
		if (!pointLightComponent->enabled)
		{
			--dynamicUniformData.numberOfPointLights;

			continue;
		}

		dynamicUniformData.pointLightAttenuationDistances[counter] = pointLightComponent->attenuationDistance;
		dynamicUniformData.pointLightIntensities[counter] = pointLightComponent->intensity;
		dynamicUniformData.pointLightColors[counter] = pointLightComponent->color;
		dynamicUniformData.pointLightWorldPositions[counter] = pointLightComponent->position;

		++counter;
	}

	counter = 0;

	const size_t numberOfSpotLightEntityComponents{ ComponentManager::GetNumberOfSpotLightComponents() };
	const SpotLightComponent *RESTRICT spotLightComponent{ ComponentManager::GetSpotLightComponents() };

	dynamicUniformData.numberOfSpotLights = numberOfSpotLightEntityComponents;

	for (size_t i = 0; i < numberOfSpotLightEntityComponents; ++i, ++spotLightComponent)
	{
		if (!spotLightComponent->enabled)
		{
			--dynamicUniformData.numberOfSpotLights;

			continue;
		}

		dynamicUniformData.spotLightAttenuationDistances[counter] = spotLightComponent->attenuationDistance;
		dynamicUniformData.spotLightIntensities[counter] = spotLightComponent->intensity;
		dynamicUniformData.spotLightInnerCutoffAngles[counter] = GameMath::CosineDegrees(spotLightComponent->innerCutoffAngle);
		dynamicUniformData.spotLightOuterCutoffAngles[counter] = GameMath::CosineDegrees(spotLightComponent->outerCutoffAngle);
		dynamicUniformData.spotLightColors[counter] = spotLightComponent->color;
		dynamicUniformData.spotLightDirections[counter] = Vector3(0.0f, -1.0f, 0.0f).Rotated(spotLightComponent->rotation);
		dynamicUniformData.spotLightDirections[counter].Y *= -1.0f;
		dynamicUniformData.spotLightWorldPositions[counter] = spotLightComponent->position;

		++counter;
	}

	//Upload the dynamic uniform data to the uniform buffer.
	uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->UploadData(static_cast<void*>(&dynamicUniformData));
}

/*
*	Updates the physical entities graphics buffers.
*/
void VulkanRenderingSystem::UpdatePhysicalEntitiesGraphicsBuffers() NOEXCEPT
{
	//Iterate over all physical entity components and update the graphics buffers.
	const size_t numberOfPhysicalEntityComponents{ ComponentManager::GetNumberOfPhysicalComponents() };
	GraphicsBufferComponent *RESTRICT graphicsBufferComponent{ ComponentManager::GetPhysicalGraphicsBufferComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetPhysicalTransformComponents() };

	for (size_t i = 0; i < numberOfPhysicalEntityComponents; ++i, ++graphicsBufferComponent, ++transformComponent)
	{
		//Calculate the model matrix.
		Matrix4 modelMatrix{ transformComponent->position, transformComponent->rotation, transformComponent->scale };

		//Upload the model matrix.
		graphicsBufferComponent->uniformBuffer.UploadData(&modelMatrix);
	}
}

/*
*	Updates the view frustum culling.
*/
void VulkanRenderingSystem::UpdateViewFrustumCulling() NOEXCEPT
{
	//Calulate the view matrix.
	Vector3 cameraWorldPosition = activeCamera->GetPosition();
	Vector3 forwardVector = activeCamera->GetForwardVector();
	Vector3 upVector = activeCamera->GetUpVector();

	Matrix4 cameraMatrix = Matrix4::LookAt(cameraWorldPosition, cameraWorldPosition + forwardVector, upVector);
	Matrix4 viewMatrix{ projectionMatrix * cameraMatrix };

	//Iterate over all physical entity components to check if they are in the view frustum.
	const size_t numberOfPhysicalEntityComponents{ ComponentManager::GetNumberOfPhysicalComponents() };
	FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetPhysicalFrustumCullingComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetPhysicalTransformComponents() };

	for (size_t i = 0; i < numberOfPhysicalEntityComponents; ++i, ++frustumCullingComponent, ++transformComponent)
	{
		//Make a local copy of the physical entity's position.
		const Vector3 position = transformComponent->position;
		const Vector3 scale = transformComponent->scale;
		const float biggestScale = GameMath::Maximum(scale.X, GameMath::Maximum(scale.Y, scale.Z));
		const float scaledExtent = frustumCullingComponent->axisAlignedBoundingBox.maximum.X * biggestScale;

		Vector4 corners[8];

		corners[0] = Vector4(-scaledExtent, -scaledExtent, -scaledExtent, 1.0f);
		corners[1] = Vector4(-scaledExtent, scaledExtent, -scaledExtent, 1.0f);
		corners[2] = Vector4(scaledExtent, scaledExtent, -scaledExtent, 1.0f);
		corners[3] = Vector4(scaledExtent, -scaledExtent, -scaledExtent, 1.0f);

		corners[4] = Vector4(-scaledExtent, -scaledExtent, scaledExtent, 1.0f);
		corners[5] = Vector4(-scaledExtent, scaledExtent, scaledExtent, 1.0f);
		corners[6] = Vector4(scaledExtent, scaledExtent, scaledExtent, 1.0f);
		corners[7] = Vector4(scaledExtent, -scaledExtent, scaledExtent, 1.0f);

		for (uint8 i = 0; i < 8; ++i)
		{
			corners[i] += Vector4(position.X, position.Y, position.Z, 0.0f);

			corners[i] = viewMatrix * corners[i];

			corners[i].X /= corners[i].W;
			corners[i].Y /= corners[i].W;
			corners[i].Z /= corners[i].W;
		}

		frustumCullingComponent->isInViewFrustum = RenderingUtilities::IsCubeWithinViewFrustum(corners);
	}
}

//Preprocessor undefunes.
#undef VULKAN_SHADERS_PATH