//Header file.
#include <GraphicsSystem.h>

//Components.
#include <ComponentManager.h>

//Entities.
#include <CameraEntity.h>
#include <PhysicalEntity.h>
#include <PointLightEntity.h>
#include <SpotLightEntity.h>
#include <TerrainEntity.h>

//Graphics.
#include <GraphicsUtilities.h>
#include <ModelLoader.h>
#include <PhysicalModel.h>
#include <ShaderLoader.h>
#include <TextureLoader.h>

//Math.
#include <GameMath.h>
#include <Matrix3.h>

//Systems.
#include <EngineSystem.h>
#include <QuestSystem.h>

//Singleton definition.
DEFINE_SINGLETON(GraphicsSystem);

/*
*	Default constructor.
*/
GraphicsSystem::GraphicsSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
GraphicsSystem::~GraphicsSystem() NOEXCEPT
{

}

/*
*	Initializes the graphics system.
*/
void GraphicsSystem::InitializeSystem() NOEXCEPT
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

	//Create all default textures.
	const byte defaultRoughness[]{ 255 };
	defaultTextures[DefaultTexture::Roughness] = VulkanInterface::Instance->Create2DTexture(1, 1, defaultRoughness);

	const byte defaultMetallic[]{ 0 };
	defaultTextures[DefaultTexture::Metallic] = VulkanInterface::Instance->Create2DTexture(1, 1, defaultMetallic);

	const byte defaultAmbientOcclusion[]{ 255 };
	defaultTextures[DefaultTexture::AmbientOcclusion] = VulkanInterface::Instance->Create2DTexture(1, 1, defaultAmbientOcclusion);
}

/*
*	Post-initializes the graphics system.
*/
void GraphicsSystem::PostInitializeSystem() NOEXCEPT
{
	//Register the graphics system update dynamic uniform data daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::GraphicsSystemUpdateDynamicUniformData, [](void *RESTRICT arguments) { static_cast<GraphicsSystem *RESTRICT>(arguments)->UpdateDynamicUniformData(); });

	//Register the graphics system update physical entities graphics buffers daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::GraphicsSystemUpdatePhysicalEntitiesGraphicsBuffers, [](void *RESTRICT arguments) { static_cast<GraphicsSystem *RESTRICT>(arguments)->UpdatePhysicalEntitiesGraphicsBuffers(); });

	//Register the graphics system update view frustum culling daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::GraphicsSystemUpdateViewFrustumCulling, [](void *RESTRICT arguments) { static_cast<GraphicsSystem *RESTRICT>(arguments)->UpdateViewFrustumCulling(); });
}

/*
*	Updates the graphics system synchronously.
*/
void GraphicsSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Carry out the graphics system update physical entities graphics buffers daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::GraphicsSystemUpdatePhysicalEntitiesGraphicsBuffers, this);

	//Carry out the graphics system asynchronous update daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::GraphicsSystemUpdateViewFrustumCulling, this);

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
*	Releases the graphics system.
*/
void GraphicsSystem::ReleaseSystem() NOEXCEPT
{
	//Release the main window.
	mainWindow.Release();

	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Creates and returns physical model.
*/
const PhysicalModel GraphicsSystem::CreatePhysicalModel(const char *RESTRICT modelPath, Vulkan2DTexture *RESTRICT albedoTexture, Vulkan2DTexture *RESTRICT normalMapTexture, Vulkan2DTexture *RESTRICT roughnessTexture, Vulkan2DTexture *RESTRICT metallicTexture, Vulkan2DTexture *RESTRICT ambientOcclusionTexture) const NOEXCEPT
{
	//Load the model.
	DynamicArray<Vertex> vertices;
	DynamicArray<uint32> indices;
	float extent{ 0.0f };

	ModelLoader::LoadModel(modelPath, vertices, indices, extent);

	//Create the vertex buffer.
	VulkanVertexBuffer *RESTRICT vertexBuffer = VulkanInterface::Instance->CreateVertexBuffer(vertices);

	//Create the index buffer.
	VulkanIndexBuffer *RESTRICT indexBuffer = VulkanInterface::Instance->CreateIndexBuffer(indices);

	//Set up the physical model.
	PhysicalModel newPhysicalModel;

	newPhysicalModel.GetAxisAlignedBoundingBox().minimum = Vector3(-extent, -extent, -extent);
	newPhysicalModel.GetAxisAlignedBoundingBox().maximum = Vector3(extent, extent, extent);
	newPhysicalModel.SetVertexBuffer(vertexBuffer);
	newPhysicalModel.SetIndexBuffer(indexBuffer);
	newPhysicalModel.GetMaterial().SetAlbedoTexture(albedoTexture);
	newPhysicalModel.GetMaterial().SetNormalMapTexture(normalMapTexture);
	newPhysicalModel.GetMaterial().SetRoughnessTexture(roughnessTexture ? roughnessTexture : defaultTextures[DefaultTexture::Roughness]);
	newPhysicalModel.GetMaterial().SetMetallicTexture(metallicTexture ? metallicTexture : defaultTextures[DefaultTexture::Metallic]);
	newPhysicalModel.GetMaterial().SetAmbientOcclusionTexture(ambientOcclusionTexture ? ambientOcclusionTexture : defaultTextures[DefaultTexture::AmbientOcclusion]);
	newPhysicalModel.SetIndexCount(static_cast<uint32>(indices.Size()));

	return newPhysicalModel;
}

/*
*	Initializes a physical entity.
*/
void GraphicsSystem::InitializePhysicalEntity(PhysicalEntity &physicalEntity, const PhysicalModel &model) const NOEXCEPT
{
	//Cache relevant data.
	VulkanDescriptorSet newDescriptorSet;
	VulkanUniformBuffer newUniformBuffer{ *CreateUniformBuffer(sizeof(Matrix4)) };
	const PhysicalMaterial &material = model.GetMaterial();
	const Vulkan2DTexture *RESTRICT albedoTexture = material.GetAlbedoTexture();
	const Vulkan2DTexture *RESTRICT normalMapTexture = material.GetNormalMapTexture();
	const Vulkan2DTexture *RESTRICT roughnessTexture = material.GetRoughnessTexture();
	const Vulkan2DTexture *RESTRICT metallicTexture = material.GetMetallicTexture();
	const Vulkan2DTexture *RESTRICT ambientOcclusionTexture = material.GetAmbientOcclusionTexture();

	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, pipelines[Pipeline::SceneBufferPipeline]->GetDescriptorSetLayout());

	//Update the write descriptor sets.
	DynamicArray<VkWriteDescriptorSet, 7> writeDescriptorSets;

	writeDescriptorSets.EmplaceUnsafe(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(newDescriptorSet, 0));
	writeDescriptorSets.EmplaceUnsafe(newUniformBuffer.GetWriteDescriptorSet(newDescriptorSet, 1));
	writeDescriptorSets.EmplaceUnsafe(albedoTexture->GetWriteDescriptorSet(newDescriptorSet, 2));
	writeDescriptorSets.EmplaceUnsafe(normalMapTexture->GetWriteDescriptorSet(newDescriptorSet, 3));
	writeDescriptorSets.EmplaceUnsafe(roughnessTexture ? roughnessTexture->GetWriteDescriptorSet(newDescriptorSet, 4) : defaultTextures[DefaultTexture::Roughness]->GetWriteDescriptorSet(newDescriptorSet, 4));
	writeDescriptorSets.EmplaceUnsafe(metallicTexture ? metallicTexture->GetWriteDescriptorSet(newDescriptorSet, 5) : defaultTextures[DefaultTexture::Metallic]->GetWriteDescriptorSet(newDescriptorSet, 5));
	writeDescriptorSets.EmplaceUnsafe(ambientOcclusionTexture ? ambientOcclusionTexture->GetWriteDescriptorSet(newDescriptorSet, 6) : defaultTextures[DefaultTexture::AmbientOcclusion]->GetWriteDescriptorSet(newDescriptorSet, 6));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Fill the physical entity components with the relevant data.
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetPhysicalEntityFrustumCullingComponents()[physicalEntity.GetComponentsIndex()] };
	GraphicsBufferComponent &graphicsBufferComponent{ ComponentManager::GetPhysicalEntityGraphicsBufferComponents()[physicalEntity.GetComponentsIndex()] };
	RenderComponent &renderComponent{ ComponentManager::GetPhysicalEntityRenderComponents()[physicalEntity.GetComponentsIndex()] };

	frustumCullingComponent.axisAlignedBoundingBox = model.GetAxisAlignedBoundingBox();
	graphicsBufferComponent.uniformBuffer = newUniformBuffer;
	renderComponent.descriptorSet = newDescriptorSet;
	renderComponent.vertexBuffer = *model.GetVertexBuffer();
	renderComponent.indexBuffer = *model.GetIndexBuffer();
	renderComponent.indexCount = model.GetIndexCount();
}

/*
*	Initializes a terrain entity.
*/
void GraphicsSystem::InitializeTerrainEntity(TerrainEntity &terrainEntity, const uint32 terrainResolution) const NOEXCEPT
{
	//Generate the plane vertices and indices.
	DynamicArray<float> terrainVertices;
	DynamicArray<uint32> terrainIndices;
	GraphicsUtilities::GeneratePlane(terrainResolution, terrainVertices, terrainIndices);

	//Create the vertex buffer.
	const void *RESTRICT terrainData[]{ terrainVertices.Data(), terrainIndices.Data() };
	const VkDeviceSize terrainDataSizes[]{ sizeof(float) * terrainVertices.Size(), sizeof(uint32) * terrainIndices.Size() };
	const VulkanBuffer terrainVertexBuffer{ *VulkanInterface::Instance->CreateBuffer(terrainData, terrainDataSizes, 2) };

	//Fill the terrain entity components with the relevant data.
	TerrainRenderComponent &terrainBufferComponent{ ComponentManager::GetTerrainEntityTerrainRenderComponents()[terrainEntity.GetComponentsIndex()] };

	//terrainBufferComponent.descriptorSet = newDescriptorSet;
	terrainBufferComponent.vertexAndIndexBuffer = terrainVertexBuffer;
	terrainBufferComponent.indexCount = static_cast<uint32>(terrainIndices.Size());
}

/*
*	Creates and returns a 2D texture.
*/
RESTRICTED Vulkan2DTexture* GraphicsSystem::Create2DTexture(const char *RESTRICT texturePath) const NOEXCEPT
{
	//Load the texture.
	int width = 0;
	int height = 0;
	int numberOfChannels = 0;
	byte *textureData = nullptr;

	TextureLoader::LoadTexture(texturePath, width, height, numberOfChannels, &textureData);

	//Create the Vulkan 2D texture.
	Vulkan2DTexture *RESTRICT new2DTexture = VulkanInterface::Instance->Create2DTexture(static_cast<uint32>(width), static_cast<uint32>(height), textureData);

	//Free the texture.
	TextureLoader::FreeTexture(textureData);

	//Return the texture.
	return new2DTexture;
}

/*
*	Creates and returns a cube map texture.
*/
RESTRICTED VulkanCubeMapTexture* GraphicsSystem::CreateCubeMapTexture(const char *RESTRICT frontTexturePath, const char *RESTRICT backTexturePath, const char *RESTRICT upTexturePath, const char *RESTRICT downTexturePath, const char *RESTRICT rightTexturePath, const char *RESTRICT leftTexturePath) const NOEXCEPT
{
	//Load all textures.
	byte *RESTRICT textureData[6];

	int width = 0;
	int height = 0;
	int numberOfChannels = 0;

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
	return newCubeMapTexture;
}

/*
*	Creates a uniform buffer and returns the identifier for the uniform buffer.
*/
RESTRICTED VulkanUniformBuffer* GraphicsSystem::CreateUniformBuffer(const size_t uniformBufferSize) const NOEXCEPT
{
	return VulkanInterface::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Sets the active camera.
*/
void GraphicsSystem::SetActiveCamera(CameraEntity *RESTRICT newActiveCamera) NOEXCEPT
{
	//Set the active camera.
	activeCamera = newActiveCamera;

	//Calculate the projection matrix.
	CalculateProjectionMatrix();
}

/*
*	Sets the active sky box cube map texture.
*/
void GraphicsSystem::SetActiveSkyBox(VulkanCubeMapTexture *RESTRICT newSkyBox) NOEXCEPT
{
	//Update the sky box texture.
	skyBoxTexture = newSkyBox;

	//Allocate the sky box descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(skyBoxDescriptorSet, pipelines[Pipeline::CubeMapPipeline]->GetDescriptorSetLayout());

	//Update the write descriptor sets.
	DynamicArray<VkWriteDescriptorSet, 2> writeDescriptorSets;

	writeDescriptorSets.EmplaceUnsafe(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(skyBoxDescriptorSet, 0));
	writeDescriptorSets.EmplaceUnsafe(newSkyBox->GetWriteDescriptorSet(skyBoxDescriptorSet, 1));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Reinitialize the descriptor sets.
	ReinitializeDescriptorSets();
}

/*
*	Sets the post processing blur amount.
*/
void GraphicsSystem::SetPostProcessingBlurAmount(const float newBlurAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.blurAmount = newBlurAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing chromatic aberration amount.
*/
void GraphicsSystem::SetPostProcessingChromaticAberrationAmount(const float newChromaticAberrationAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.chromaticAberrationAmount = newChromaticAberrationAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing saturation.
*/
void GraphicsSystem::SetPostProcessingSaturation(const float newSaturation) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.saturation = newSaturation;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Sets the post processing sharpen amount.
*/
void GraphicsSystem::SetPostProcessingSharpenAmount(const float newSharpenAmount) NOEXCEPT
{
	//Update the post processing uniform data.
	postProcessingUniformData.sharpenAmount = newSharpenAmount;

	//Upload the data to the post processing data uniform buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Initializes all render targets.
*/
void GraphicsSystem::InitializeRenderTargets() NOEXCEPT
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
void GraphicsSystem::InitializeSemaphores() NOEXCEPT
{
	//Initialize all semaphores.
	semaphores[Semaphore::ImageAvailable] = VulkanInterface::Instance->CreateSemaphore();
	semaphores[Semaphore::RenderFinished] = VulkanInterface::Instance->CreateSemaphore();
}

/*
*	Initializes all uniform buffers.
*/
void GraphicsSystem::InitializeUniformBuffers() NOEXCEPT
{
	//Create the dynamic uniform data buffer.
	uniformBuffers[UniformBuffer::DynamicUniformDataBuffer] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(DynamicUniformData));

	//Create the post processing uniform data buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(PostProcessingUniformData));

	//Upload the initial data to the post processing uniform data buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&postProcessingUniformData);
}

/*
*	Initializes all shader modules.
*/
void GraphicsSystem::InitializeShaderModules() NOEXCEPT
{
	//Initialize the cube map fragment shader module.
	const auto cubeMapFragmentShaderByteCode = ShaderLoader::LoadShader("CubeMapFragmentShader.spv");
	shaderModules[ShaderModule::CubeMapFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(cubeMapFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the cube map vertex shader module.
	const auto cubeMapVertexShaderByteCode = ShaderLoader::LoadShader("CubeMapVertexShader.spv");
	shaderModules[ShaderModule::CubeMapVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(cubeMapVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the lighting fragment shader module.
	const auto lightingFragmentShaderByteCode = ShaderLoader::LoadShader("LightingFragmentShader.spv");
	shaderModules[ShaderModule::LightingFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(lightingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the post processing fragment shader module.
	const auto postProcessingFragmentShaderByteCode = ShaderLoader::LoadShader("PostProcessingFragmentShader.spv");
	shaderModules[ShaderModule::PostProcessingFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(postProcessingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the scene buffer fragment shader module.
	const auto sceneBufferFragmentShaderByteCode = ShaderLoader::LoadShader("SceneBufferFragmentShader.spv");
	shaderModules[ShaderModule::SceneBufferFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(sceneBufferFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the scene buffer vertex shader module.
	const auto sceneBufferVertexShaderByteCode = ShaderLoader::LoadShader("SceneBufferVertexShader.spv");
	shaderModules[ShaderModule::SceneBufferVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(sceneBufferVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the lighting vertex shader module.
	const auto viewportVertexShaderByteCode = ShaderLoader::LoadShader("ViewportVertexShader.spv");
	shaderModules[ShaderModule::ViewportVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(viewportVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);
}

/*
*	Initializes all pipelines.
*/
void GraphicsSystem::InitializePipelines() NOEXCEPT
{
	//Create the scene buffer pipeline.
	VulkanPipelineCreationParameters sceneBufferPipelineCreationParameters;

	sceneBufferPipelineCreationParameters.shaderModules.Reserve(2);
	sceneBufferPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::SceneBufferVertexShaderModule]);
	sceneBufferPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::SceneBufferFragmentShaderModule]);
	sceneBufferPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(7);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	sceneBufferPipelineCreationParameters.depthBuffers.Reserve(1);
	sceneBufferPipelineCreationParameters.depthBuffers.EmplaceUnsafe(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
	sceneBufferPipelineCreationParameters.colorAttachments.Resize(1);
	sceneBufferPipelineCreationParameters.colorAttachments[0].Reserve(3);
	sceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetImageView());
	sceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget]->GetImageView());
	sceneBufferPipelineCreationParameters.colorAttachments[0].EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget]->GetImageView());
	sceneBufferPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_CLEAR;
	sceneBufferPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	sceneBufferPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	sceneBufferPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	sceneBufferPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	sceneBufferPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	sceneBufferPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	sceneBufferPipelineCreationParameters.depthTestEnable = VK_TRUE;
	sceneBufferPipelineCreationParameters.depthWriteEnable = VK_TRUE;
	sceneBufferPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;

	pipelines[Pipeline::SceneBufferPipeline] = VulkanInterface::Instance->CreatePipeline(sceneBufferPipelineCreationParameters);

	//Create the physical pipeline.
	VulkanPipelineCreationParameters lightingPipelineCreationParameters;

	lightingPipelineCreationParameters.shaderModules.Reserve(2);
	lightingPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::ViewportVertexShaderModule]);
	lightingPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::LightingFragmentShaderModule]);
	lightingPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(7);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	lightingPipelineCreationParameters.colorAttachments.Resize(1);
	lightingPipelineCreationParameters.colorAttachments[0].Reserve(1);
	lightingPipelineCreationParameters.colorAttachments[0].EmplaceUnsafe(renderTargets[RenderTarget::SceneRenderTarget]->GetImageView());
	lightingPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
	lightingPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	lightingPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	lightingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	lightingPipelineCreationParameters.depthTestEnable = VK_FALSE;
	lightingPipelineCreationParameters.depthWriteEnable = VK_FALSE;
	lightingPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;

	pipelines[Pipeline::LightingPipeline] = VulkanInterface::Instance->CreatePipeline(lightingPipelineCreationParameters);

	//Create the cube map pipeline.
	VulkanPipelineCreationParameters cubeMapPipelineCreationParameters;

	cubeMapPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
	cubeMapPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	cubeMapPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	cubeMapPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	cubeMapPipelineCreationParameters.depthBuffers.Reserve(1);
	cubeMapPipelineCreationParameters.depthBuffers.EmplaceUnsafe(depthBuffers[DepthBuffer::SceneBufferDepthBuffer]);
	cubeMapPipelineCreationParameters.colorAttachments.Resize(1);
	cubeMapPipelineCreationParameters.colorAttachments[0].Reserve(1);
	cubeMapPipelineCreationParameters.colorAttachments[0].EmplaceUnsafe(renderTargets[RenderTarget::SceneRenderTarget]->GetImageView());
	cubeMapPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	cubeMapPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	cubeMapPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	cubeMapPipelineCreationParameters.depthTestEnable = VK_TRUE;
	cubeMapPipelineCreationParameters.depthWriteEnable = VK_TRUE;
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(2);
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	cubeMapPipelineCreationParameters.shaderModules.Reserve(2);
	cubeMapPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::CubeMapVertexShaderModule]);
	cubeMapPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::CubeMapFragmentShaderModule]);
	cubeMapPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
		postProcessingPipelineCreationParameters.colorAttachments[i].EmplaceUnsafe(swapchainImageViews[i]);
	}

	postProcessingPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	postProcessingPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	postProcessingPipelineCreationParameters.depthCompareOp = VK_COMPARE_OP_LESS;
	postProcessingPipelineCreationParameters.depthTestEnable = VK_FALSE;
	postProcessingPipelineCreationParameters.depthWriteEnable = VK_FALSE;
	postProcessingPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(3);
	postProcessingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	postProcessingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	postProcessingPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	postProcessingPipelineCreationParameters.shaderModules.Reserve(2);
	postProcessingPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::ViewportVertexShaderModule]);
	postProcessingPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::PostProcessingFragmentShaderModule]);
	postProcessingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	postProcessingPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

	pipelines[Pipeline::PostProcessingPipeline] = VulkanInterface::Instance->CreatePipeline(postProcessingPipelineCreationParameters);
}

/*
*	Initializes all descriptor sets.
*/
void GraphicsSystem::InitializeDescriptorSets() NOEXCEPT
{
	{
		//Initialize the lighting descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], pipelines[Pipeline::LightingPipeline]->GetDescriptorSetLayout());

		//Update the write descriptor sets.
		DynamicArray<VkWriteDescriptorSet, 5> writeDescriptorSets;

		writeDescriptorSets.EmplaceUnsafe(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 0));
		writeDescriptorSets.EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 1));
		writeDescriptorSets.EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 2));
		writeDescriptorSets.EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 3));
		writeDescriptorSets.EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 4));

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the post processing descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], pipelines[Pipeline::PostProcessingPipeline]->GetDescriptorSetLayout());

		//Update the write descriptor sets.
		DynamicArray<VkWriteDescriptorSet, 3> writeDescriptorSets;

		writeDescriptorSets.EmplaceUnsafe(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], 0));
		writeDescriptorSets.EmplaceUnsafe(uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], 1));
		writeDescriptorSets.EmplaceUnsafe(renderTargets[RenderTarget::SceneRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::PostProcessingDescriptorSet], 2));

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}
}

/*
*	Calculates the projection matrix.
*/
void GraphicsSystem::CalculateProjectionMatrix() NOEXCEPT
{
	//Calculate the projection matrix.
	projectionMatrix = Matrix4::Perspective(GameMath::DegreesToRadians(activeCamera->GetFieldOfView()), 1920.0f / 1080.0f, activeCamera->GetNearPlane(), activeCamera->GetFarPlane());
}

/*
*	Begins the frame.
*/
void GraphicsSystem::BeginFrame() NOEXCEPT
{
	//Wait for the graphics queue to finish.
	VulkanInterface::Instance->GetGraphicsQueue().WaitIdle();

	//Carry out the graphics system update dynamic uniform data daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::GraphicsSystemUpdateDynamicUniformData, this);

	//Set the current command buffer.
	currentSwapchainCommandBuffer = VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();

	//Set up the current command buffer.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBindPipeline(*pipelines[Pipeline::SceneBufferPipeline]);
	swapchainCommandBuffers[currentSwapchainCommandBuffer].CommandBeginRenderPass(pipelines[Pipeline::SceneBufferPipeline]->GetRenderPass(), 0, true, 4);
}

/*
*	Renders all physical entities.
*/
void GraphicsSystem::RenderPhysicalEntities() NOEXCEPT
{
	//Wait for the physical entity update daily group quest to complete.
	//QuestSystem::Instance->WaitForDailyQuest(DailyQuests::GraphicsSystemUpdateViewFrustumCulling);

	//Cache the pipeline.
	VulkanPipeline &sceneBufferPipeline{ *pipelines[Pipeline::SceneBufferPipeline] };
	VulkanCommandBuffer &commandBuffer{ swapchainCommandBuffers[currentSwapchainCommandBuffer] };

	//Iterate over all physical entity components and draw them all.
	const size_t numberOfPhysicalEntityComponents{ ComponentManager::GetNumberOfPhysicalEntityComponents() };
	const FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetPhysicalEntityFrustumCullingComponents() };
	const RenderComponent *RESTRICT renderComponent{ ComponentManager::GetPhysicalEntityRenderComponents() };

	for (size_t i = 0; i < numberOfPhysicalEntityComponents; ++i, ++frustumCullingComponent, ++renderComponent)
	{
		//Don't draw this physical entity if it isn't in the view frustum.
		if (!frustumCullingComponent->isInViewFrustum)
			continue;

		commandBuffer.CommandBindDescriptorSets(sceneBufferPipeline, renderComponent->descriptorSet);
		commandBuffer.CommandBindVertexBuffers(renderComponent->vertexBuffer);
		commandBuffer.CommandBindIndexBuffer(renderComponent->indexBuffer);
		commandBuffer.CommandDrawIndexed(renderComponent->indexCount);
	}

	//End the render pass.
	commandBuffer.CommandEndRenderPass();
}

/*
*	Renders lighting.
*/
void GraphicsSystem::RenderLighting() NOEXCEPT
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
void GraphicsSystem::RenderSkyBox() NOEXCEPT
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
void GraphicsSystem::RenderPostProcessing() NOEXCEPT
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
void GraphicsSystem::EndFrame() NOEXCEPT
{
	//Set up the proper parameters.
	static const DynamicArray<VkSemaphore> waitSemaphores{ semaphores[Semaphore::ImageAvailable]->Get() };
	static const VkPipelineStageFlags waitStages{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	static const DynamicArray<VkSemaphore> signalSemaphores{ semaphores[Semaphore::RenderFinished]->Get() };

	//End the current command buffer.
	swapchainCommandBuffers[currentSwapchainCommandBuffer].End();

	//Wait for the graphics system update dynamic uniform data daily quest to finish.
	QuestSystem::Instance->WaitForDailyQuest(DailyQuests::GraphicsSystemUpdateDynamicUniformData);

	//Wait for the graphics system update physical entities graphics buffers daily quest to finish.
	QuestSystem::Instance->WaitForDailyQuest(DailyQuests::GraphicsSystemUpdatePhysicalEntitiesGraphicsBuffers);

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(swapchainCommandBuffers[currentSwapchainCommandBuffer], waitSemaphores, waitStages, signalSemaphores);
}

/*
*	Re-initializes all descriptor sets.
*/
void GraphicsSystem::ReinitializeDescriptorSets() NOEXCEPT
{
	//Update the write descriptor sets.
	DynamicArray<VkWriteDescriptorSet, 5> writeDescriptorSets;

	writeDescriptorSets.EmplaceUnsafe(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 0));
	writeDescriptorSets.EmplaceUnsafe(skyBoxTexture->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 1));
	writeDescriptorSets.EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 2));
	writeDescriptorSets.EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 3));
	writeDescriptorSets.EmplaceUnsafe(renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget]->GetWriteDescriptorSet(descriptorSets[DescriptorSet::LightingDescriptorSet], 4));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
}

/*
*	Updates the dynamic uniform data.
*/
void GraphicsSystem::UpdateDynamicUniformData() NOEXCEPT
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

	dynamicUniformData.inverseCameraMatrix = inverseCameraMatrix;
	dynamicUniformData.inverseProjectionMatrix = inverseProjectionMatrix;
	dynamicUniformData.originViewMatrix = projectionMatrix * cameraOriginMatrix;
	dynamicUniformData.viewMatrix = viewMatrix;
	dynamicUniformData.cameraWorldPosition = cameraWorldPosition;

	const size_t numberOfDirectionalLightEntityComponents{ ComponentManager::GetNumberOfDirectionalLightEntityComponents() };

	if (numberOfDirectionalLightEntityComponents > 0)
	{
		const DirectionalLightComponent *RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightEntityDirectionalLightComponents() };

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

	const size_t numberOfPointLightEntityComponents{ ComponentManager::GetNumberOfPointLightEntityComponents() };
	const PointLightComponent *RESTRICT pointLightComponent{ ComponentManager::GetPointLightEntityPointLightComponents() };

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

	const size_t numberOfSpotLightEntityComponents{ ComponentManager::GetNumberOfSpotLightEntityComponents() };
	const SpotLightComponent *RESTRICT spotLightComponent{ ComponentManager::GetSpotLightEntitySpotLightComponents() };

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
void GraphicsSystem::UpdatePhysicalEntitiesGraphicsBuffers() NOEXCEPT
{
	//Iterate over all physical entity components and update the graphics buffers.
	const size_t numberOfPhysicalEntityComponents{ ComponentManager::GetNumberOfPhysicalEntityComponents() };
	GraphicsBufferComponent *RESTRICT graphicsBufferComponent{ ComponentManager::GetPhysicalEntityGraphicsBufferComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetPhysicalEntityTransformComponents() };

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
void GraphicsSystem::UpdateViewFrustumCulling() NOEXCEPT
{
	//Calulate the view matrix.
	Vector3 cameraWorldPosition = activeCamera->GetPosition();
	Vector3 forwardVector = activeCamera->GetForwardVector();
	Vector3 upVector = activeCamera->GetUpVector();

	Matrix4 cameraMatrix = Matrix4::LookAt(cameraWorldPosition, cameraWorldPosition + forwardVector, upVector);
	Matrix4 viewMatrix{ projectionMatrix * cameraMatrix };

	//Iterate over all physical entity components to check if they are in the view frustum.
	const size_t numberOfPhysicalEntityComponents{ ComponentManager::GetNumberOfPhysicalEntityComponents() };
	FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetPhysicalEntityFrustumCullingComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetPhysicalEntityTransformComponents() };

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

		frustumCullingComponent->isInViewFrustum = GraphicsUtilities::IsCubeWithinViewFrustum(corners);
	}
}