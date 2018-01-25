//Header file.
#include <GraphicsSystem.h>

//Entities.
#include <CameraEntity.h>
#include <PhysicalEntity.h>
#include <PointLightEntity.h>
#include <SpotLightEntity.h>

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
GraphicsSystem::GraphicsSystem() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
GraphicsSystem::~GraphicsSystem() CATALYST_NOEXCEPT
{

}

/*
*	Initializes the graphics system.
*/
void GraphicsSystem::InitializeSystem() CATALYST_NOEXCEPT
{
	//Initialize the main window.
	mainWindow.Initialize();

	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize(mainWindow);

	//Initialize all render targets.
	InitializeRenderTargets();

	//Resize the number of command buffers to be equal to the amount of swapchain images.
	commandBuffers.Resize(VulkanInterface::Instance->GetSwapchain().GetSwapChainImages().Size());

	//Allocate all command buffers.
	for (auto &commandBuffer : commandBuffers)
	{
		VulkanInterface::Instance->GetGraphicsCommandPool().AllocateVulkanCommandBuffer(commandBuffer);
	}

	//Create the dynamic uniform data buffer.
	uniformBuffers[UniformBuffer::DynamicUniformDataBuffer] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(DynamicUniformData));

	//Initialize all shader modules.
	InitializeShaderModules();

	//Initialize all pipelines.
	InitializePipelines();

	//Initialize all descriptor sets.
	InitializeDescriptorSets();

	//Initialize all semaphores.
	semaphores[Semaphore::ImageAvailable] = VulkanInterface::Instance->CreateSemaphore();
	semaphores[Semaphore::RenderFinished] = VulkanInterface::Instance->CreateSemaphore();

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
void GraphicsSystem::PostInitializeSystem() CATALYST_NOEXCEPT
{
	//Register the graphics system update dynamic uniform data daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::GraphicsSystemUpdateDynamicUniformData, [](void *CATALYST_RESTRICT arguments) { static_cast<GraphicsSystem *CATALYST_RESTRICT>(arguments)->UpdateDynamicUniformData(); });

	//Register the graphics system asynchronous update daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::GraphicsSystemUpdateViewFrustumCulling, [](void *CATALYST_RESTRICT arguments) { static_cast<GraphicsSystem *CATALYST_RESTRICT>(arguments)->UpdateViewFrustumCulling(); });

	//Register the physical entity update daily group quest.
	QuestSystem::Instance->RegisterDailyGroupQuest(DailyGroupQuests::GraphicsSystemPhysicalEntityUpdate, [](void *CATALYST_RESTRICT, void *CATALYST_RESTRICT element)
	{
		PhysicalEntity *CATALYST_RESTRICT physicalEntity = *static_cast<PhysicalEntity *CATALYST_RESTRICT *CATALYST_RESTRICT>(element);

		physicalEntity->UpdateModelMatrix();
	});
}

/*
*	Updates the graphics system synchronously.
*/
void GraphicsSystem::UpdateSystemSynchronous() CATALYST_NOEXCEPT
{
	//Carry out the graphics system asynchronous update daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::GraphicsSystemUpdateViewFrustumCulling, this);

	//Carry out the physical entity update daily group quest.
	QuestSystem::Instance->CarryOutDailyGroupQuest(DailyGroupQuests::GraphicsSystemPhysicalEntityUpdate, nullptr, PhysicalEntity::physicalEntities.Data(), PhysicalEntity::physicalEntities.Size(), sizeof(PhysicalEntity*));

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

	//End the frame.
	EndFrame();

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(semaphores[Semaphore::RenderFinished]);
}

/*
*	Releases the graphics system.
*/
void GraphicsSystem::ReleaseSystem() CATALYST_NOEXCEPT
{
	//Release the main window.
	mainWindow.Release();

	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Creates a physical descriptor set.
*/
void GraphicsSystem::CreatePhysicalDescriptorSet(VulkanDescriptorSet &vulkanDescriptorSet, const VulkanUniformBuffer *CATALYST_RESTRICT modelDataUniformBuffer, const Vulkan2DTexture *CATALYST_RESTRICT albedoTexture, const Vulkan2DTexture *CATALYST_RESTRICT normalMapTexture, const Vulkan2DTexture *CATALYST_RESTRICT roughnessTexture, const Vulkan2DTexture *CATALYST_RESTRICT metallicTexture, const Vulkan2DTexture *CATALYST_RESTRICT ambientOcclusionTexture) const CATALYST_NOEXCEPT
{
	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(vulkanDescriptorSet, pipelines[Pipeline::SceneBufferPipeline]->GetDescriptorSetLayout());

	//Update the write descriptor sets.
	DynamicArray<VkWriteDescriptorSet, 7> writeDescriptorSets;

	writeDescriptorSets.EmplaceUnsafe(uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->GetWriteDescriptorSet(vulkanDescriptorSet, 0));
	writeDescriptorSets.EmplaceUnsafe(modelDataUniformBuffer->GetWriteDescriptorSet(vulkanDescriptorSet, 1));
	writeDescriptorSets.EmplaceUnsafe(albedoTexture->GetWriteDescriptorSet(vulkanDescriptorSet, 2));
	writeDescriptorSets.EmplaceUnsafe(normalMapTexture->GetWriteDescriptorSet(vulkanDescriptorSet, 3));
	writeDescriptorSets.EmplaceUnsafe(roughnessTexture ? roughnessTexture->GetWriteDescriptorSet(vulkanDescriptorSet, 4) : defaultTextures[DefaultTexture::Roughness]->GetWriteDescriptorSet(vulkanDescriptorSet, 4));
	writeDescriptorSets.EmplaceUnsafe(metallicTexture ? metallicTexture->GetWriteDescriptorSet(vulkanDescriptorSet, 5) : defaultTextures[DefaultTexture::Metallic]->GetWriteDescriptorSet(vulkanDescriptorSet, 5));
	writeDescriptorSets.EmplaceUnsafe(ambientOcclusionTexture ? ambientOcclusionTexture->GetWriteDescriptorSet(vulkanDescriptorSet, 6) : defaultTextures[DefaultTexture::AmbientOcclusion]->GetWriteDescriptorSet(vulkanDescriptorSet, 6));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
}

/*
*	Creates and returns physical model.
*/
const PhysicalModel GraphicsSystem::CreatePhysicalModel(const char *CATALYST_RESTRICT modelPath, Vulkan2DTexture *CATALYST_RESTRICT albedoTexture, Vulkan2DTexture *CATALYST_RESTRICT normalMapTexture, Vulkan2DTexture *CATALYST_RESTRICT roughnessTexture, Vulkan2DTexture *CATALYST_RESTRICT metallicTexture, Vulkan2DTexture *CATALYST_RESTRICT ambientOcclusionTexture) const CATALYST_NOEXCEPT
{
	//Load the model.
	DynamicArray<Vertex> vertices;
	DynamicArray<uint32> indices;
	float extent{ 0.0f };

	ModelLoader::LoadModel(modelPath, vertices, indices, extent);

	//Create the vertex buffer.
	VulkanVertexBuffer *CATALYST_RESTRICT vertexBuffer = VulkanInterface::Instance->CreateVertexBuffer(vertices);

	//Create the index buffer.
	VulkanIndexBuffer *CATALYST_RESTRICT indexBuffer = VulkanInterface::Instance->CreateIndexBuffer(indices);

	//Set up the physical model.
	PhysicalModel newPhysicalModel;

	newPhysicalModel.SetExtent(extent);
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
*	Creates and returns a 2D texture.
*/
CATALYST_RESTRICTED Vulkan2DTexture* GraphicsSystem::Create2DTexture(const char *CATALYST_RESTRICT texturePath) const CATALYST_NOEXCEPT
{
	//Load the texture.
	int width = 0;
	int height = 0;
	int numberOfChannels = 0;
	byte *textureData = nullptr;

	TextureLoader::LoadTexture(texturePath, width, height, numberOfChannels, &textureData);

	//Create the Vulkan 2D texture.
	Vulkan2DTexture *CATALYST_RESTRICT new2DTexture = VulkanInterface::Instance->Create2DTexture(static_cast<uint32>(width), static_cast<uint32>(height), textureData);

	//Free the texture.
	TextureLoader::FreeTexture(textureData);

	//Return the texture.
	return new2DTexture;
}

/*
*	Creates and returns a cube map texture.
*/
CATALYST_RESTRICTED VulkanCubeMapTexture* GraphicsSystem::CreateCubeMapTexture(const char *CATALYST_RESTRICT frontTexturePath, const char *CATALYST_RESTRICT backTexturePath, const char *CATALYST_RESTRICT upTexturePath, const char *CATALYST_RESTRICT downTexturePath, const char *CATALYST_RESTRICT rightTexturePath, const char *CATALYST_RESTRICT leftTexturePath) const CATALYST_NOEXCEPT
{
	//Load all textures.
	byte *CATALYST_RESTRICT textureData[6];

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
	VulkanCubeMapTexture *CATALYST_RESTRICT newCubeMapTexture = VulkanInterface::Instance->CreateCubeMapTexture(static_cast<uint32>(width), static_cast<uint32>(height), reinterpret_cast<const byte *CATALYST_RESTRICT *CATALYST_RESTRICT>(&textureData));

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
CATALYST_RESTRICTED VulkanUniformBuffer* GraphicsSystem::CreateUniformBuffer(const size_t uniformBufferSize) const CATALYST_NOEXCEPT
{
	return VulkanInterface::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Sets the active camera.
*/
void GraphicsSystem::SetActiveCamera(CameraEntity *CATALYST_RESTRICT newActiveCamera) CATALYST_NOEXCEPT
{
	//Set the active camera.
	activeCamera = newActiveCamera;

	//Calculate the projection matrix.
	CalculateProjectionMatrix();
}

/*
*	Sets the active sky box cube map texture.
*/
void GraphicsSystem::SetActiveSkyBox(VulkanCubeMapTexture *CATALYST_RESTRICT newSkyBox) CATALYST_NOEXCEPT
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
}

/*
*	Initializes all render targets.
*/
void GraphicsSystem::InitializeRenderTargets() CATALYST_NOEXCEPT
{
	//Initialize all depth buffers.
	depthBuffers[DepthBuffer::SceneBufferDepthBuffer] = VulkanInterface::Instance->CreateDepthBuffer(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Initialize all render targets.
	renderTargets[RenderTarget::SceneBufferAlbedoColorRenderTarget] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[RenderTarget::SceneBufferNormalDirectionDepthRenderTarget] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
	renderTargets[RenderTarget::SceneBufferRoughnessMetallicAmbientOcclusionRenderTarget] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());
}

/*
*	Initializes all shader modules.
*/
void GraphicsSystem::InitializeShaderModules() CATALYST_NOEXCEPT
{
	//Initialize the scene buffer vertex shader module.
	const auto sceneBufferVertexShaderByteCode = ShaderLoader::LoadShader("SceneBufferVertexShader.spv");
	shaderModules[ShaderModule::SceneBufferVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(sceneBufferVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the scene buffer fragment shader module.
	const auto sceneBufferFragmentShaderByteCode = ShaderLoader::LoadShader("SceneBufferFragmentShader.spv");
	shaderModules[ShaderModule::SceneBufferFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(sceneBufferFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the lighting vertex shader module.
	const auto lightingVertexShaderByteCode = ShaderLoader::LoadShader("LightingVertexShader.spv");
	shaderModules[ShaderModule::LightingVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(lightingVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the lighting fragment shader module.
	const auto lightingFragmentShaderByteCode = ShaderLoader::LoadShader("LightingFragmentShader.spv");
	shaderModules[ShaderModule::LightingFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(lightingFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//Initialize the cube map vertex shader module.
	const auto cubeMapVertexShaderByteCode = ShaderLoader::LoadShader("CubeMapVertexShader.spv");
	shaderModules[ShaderModule::CubeMapVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(cubeMapVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the cube map fragment shader module.
	const auto cubeMapFragmentShaderByteCode = ShaderLoader::LoadShader("CubeMapFragmentShader.spv");
	shaderModules[ShaderModule::CubeMapFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(cubeMapFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
}

/*
*	Initializes all pipelines.
*/
void GraphicsSystem::InitializePipelines() CATALYST_NOEXCEPT
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
	sceneBufferPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR;
	sceneBufferPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	sceneBufferPipelineCreationParameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	sceneBufferPipelineCreationParameters.colorAttachmentFormat = VK_FORMAT_R8G8B8A8_UNORM;
	sceneBufferPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	sceneBufferPipelineCreationParameters.depthTestEnable = VK_TRUE;
	sceneBufferPipelineCreationParameters.depthWriteEnable = VK_TRUE;

	pipelines[Pipeline::SceneBufferPipeline] = VulkanInterface::Instance->CreatePipeline(sceneBufferPipelineCreationParameters);

	//Create the physical pipeline.
	VulkanPipelineCreationParameters lightingPipelineCreationParameters;

	lightingPipelineCreationParameters.shaderModules.Reserve(2);
	lightingPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::LightingVertexShaderModule]);
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

	const DynamicArray<VkImageView> &swapchainImageViews = VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews();
	const size_t swapchainImageViewsSize = swapchainImageViews.Size();
	const VulkanDepthBuffer &depthBuffer = VulkanInterface::Instance->GetSwapchain().GetDepthBuffer();

	lightingPipelineCreationParameters.depthBuffers.Reserve(swapchainImageViewsSize);
	lightingPipelineCreationParameters.colorAttachments.Resize(swapchainImageViewsSize);

	for (size_t i = 0; i < swapchainImageViewsSize; ++i)
	{
		lightingPipelineCreationParameters.colorAttachments[i].Reserve(1);
		lightingPipelineCreationParameters.depthBuffers.EmplaceUnsafe(&depthBuffer);
		lightingPipelineCreationParameters.colorAttachments[i].EmplaceUnsafe(swapchainImageViews[i]);
	}

	lightingPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
	lightingPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	lightingPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	lightingPipelineCreationParameters.colorAttachmentFinalLayout = VULKAN_IMAGE_LAYOUT_PRESENT_SRC;
	lightingPipelineCreationParameters.colorAttachmentFormat = VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format;
	lightingPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	lightingPipelineCreationParameters.depthTestEnable = VK_FALSE;
	lightingPipelineCreationParameters.depthWriteEnable = VK_FALSE;

	pipelines[Pipeline::LightingPipeline] = VulkanInterface::Instance->CreatePipeline(lightingPipelineCreationParameters);

	//Create the cube map pipeline.
	VulkanPipelineCreationParameters cubeMapPipelineCreationParameters;

	cubeMapPipelineCreationParameters.shaderModules.Reserve(2);
	cubeMapPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::CubeMapVertexShaderModule]);
	cubeMapPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::CubeMapFragmentShaderModule]);
	cubeMapPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(2);
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	cubeMapPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	cubeMapPipelineCreationParameters.depthBuffers.Reserve(swapchainImageViewsSize);
	cubeMapPipelineCreationParameters.colorAttachments.Resize(swapchainImageViewsSize);

	for (size_t i = 0; i < swapchainImageViewsSize; ++i)
	{
		cubeMapPipelineCreationParameters.depthBuffers.EmplaceUnsafe(&depthBuffer);
		cubeMapPipelineCreationParameters.colorAttachments[i].Reserve(1);
		cubeMapPipelineCreationParameters.colorAttachments[i].EmplaceUnsafe(swapchainImageViews[i]);
	}

	cubeMapPipelineCreationParameters.attachmentLoadOperator = VK_ATTACHMENT_LOAD_OP_LOAD;
	cubeMapPipelineCreationParameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	cubeMapPipelineCreationParameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	cubeMapPipelineCreationParameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	cubeMapPipelineCreationParameters.colorAttachmentFinalLayout = VULKAN_IMAGE_LAYOUT_PRESENT_SRC;
	cubeMapPipelineCreationParameters.colorAttachmentFormat = VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format;
	cubeMapPipelineCreationParameters.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	cubeMapPipelineCreationParameters.depthTestEnable = VK_TRUE;
	cubeMapPipelineCreationParameters.depthWriteEnable = VK_TRUE;

	pipelines[Pipeline::CubeMapPipeline] = VulkanInterface::Instance->CreatePipeline(cubeMapPipelineCreationParameters);
}

/*
*	Initializes all descriptor sets.
*/
void GraphicsSystem::InitializeDescriptorSets() CATALYST_NOEXCEPT
{
	//Initialize the scene buffer descriptor set.
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

/*
*	Calculates the projection matrix.
*/
void GraphicsSystem::CalculateProjectionMatrix() CATALYST_NOEXCEPT
{
	//Calculate the projection matrix.
	projectionMatrix = Matrix4::Perspective(GameMath::DegreesToRadians(activeCamera->GetFieldOfView()), 1920.0f / 1080.0f, activeCamera->GetNearPlane(), activeCamera->GetFarPlane());
}

/*
*	Begins the frame.
*/
void GraphicsSystem::BeginFrame() CATALYST_NOEXCEPT
{
	//Wait for the graphics queue to finish.
	VulkanInterface::Instance->GetGraphicsQueue().WaitIdle();

	//Carry out the graphics system update dynamic uniform data daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::GraphicsSystemUpdateDynamicUniformData, this);

	//Set the current command buffer.
	currentCommandBuffer = VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();

	//Set up the current command buffer.
	commandBuffers[currentCommandBuffer].Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	commandBuffers[currentCommandBuffer].CommandBindPipeline(*pipelines[Pipeline::SceneBufferPipeline]);
	commandBuffers[currentCommandBuffer].CommandBeginRenderPass(pipelines[Pipeline::SceneBufferPipeline]->GetRenderPass(), 0, 4);
}

/*
*	Renders all physical entities.
*/
void GraphicsSystem::RenderPhysicalEntities() CATALYST_NOEXCEPT
{
	//Wait for the physical entity update daily group quest to complete.
	QuestSystem::Instance->WaitForDailyGroupQuest(DailyGroupQuests::GraphicsSystemPhysicalEntityUpdate);

	//Iterate over all physical entities and draw them all.
	for (PhysicalEntity * CATALYST_RESTRICT physicalEntity : PhysicalEntity::physicalEntities)
	{
		//Don't draw this physical entity if it isn't in the view frustum.
		if (!physicalEntity->IsInViewFrustum())
			continue;

		commandBuffers[currentCommandBuffer].CommandBindDescriptorSets(*pipelines[Pipeline::SceneBufferPipeline], physicalEntity->GetDescriptorSet());
		commandBuffers[currentCommandBuffer].CommandBindVertexBuffers(*physicalEntity->GetModel().GetVertexBuffer());
		commandBuffers[currentCommandBuffer].CommandBindIndexBuffer(*physicalEntity->GetModel().GetIndexBuffer());
		commandBuffers[currentCommandBuffer].CommandDrawIndexed(physicalEntity->GetModel().GetIndexCount());
	}

	//End the render pass.
	commandBuffers[currentCommandBuffer].CommandEndRenderPass();
}

/*
*	Renders lighting.
*/
void GraphicsSystem::RenderLighting() CATALYST_NOEXCEPT
{
	//Bind the lighting pipeline.
	commandBuffers[currentCommandBuffer].CommandBindPipeline(*pipelines[Pipeline::LightingPipeline]);

	//Bind the lighting render pass.
	commandBuffers[currentCommandBuffer].CommandBeginRenderPass(pipelines[Pipeline::LightingPipeline]->GetRenderPass(), currentCommandBuffer, 2);

	//Bind the scene buffer descriptor set.
	commandBuffers[currentCommandBuffer].CommandBindDescriptorSets(*pipelines[Pipeline::LightingPipeline], descriptorSets[DescriptorSet::LightingDescriptorSet]);

	//Draw the viewport!
	commandBuffers[currentCommandBuffer].CommandDraw(4);

	//End the render pass.
	commandBuffers[currentCommandBuffer].CommandEndRenderPass();
}

/*
*	Renders sky box.
*/
void GraphicsSystem::RenderSkyBox() CATALYST_NOEXCEPT
{
	//Bind the cube map pipeline.
	commandBuffers[currentCommandBuffer].CommandBindPipeline(*pipelines[Pipeline::CubeMapPipeline]);

	//Bind the cube map render pass.
	commandBuffers[currentCommandBuffer].CommandBeginRenderPass(pipelines[Pipeline::CubeMapPipeline]->GetRenderPass(), currentCommandBuffer, false);

	//Bind the sky box descriptor set.
	commandBuffers[currentCommandBuffer].CommandBindDescriptorSets(*pipelines[Pipeline::CubeMapPipeline], skyBoxDescriptorSet);

	//Draw the sky box!
	commandBuffers[currentCommandBuffer].CommandDraw(36);

	//End the cube map render pass.
	commandBuffers[currentCommandBuffer].CommandEndRenderPass();
}

/*
*	Ends the frame.
*/
void GraphicsSystem::EndFrame() CATALYST_NOEXCEPT
{
	//Set up the proper parameters.
	static const DynamicArray<VkSemaphore> waitSemaphores{ semaphores[Semaphore::ImageAvailable]->Get() };
	static const VkPipelineStageFlags waitStages{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	static const DynamicArray<VkSemaphore> signalSemaphores{ semaphores[Semaphore::RenderFinished]->Get() };

	//End the current command buffer.
	commandBuffers[currentCommandBuffer].End();

	//Wait for the graphics system update dynamic uniform data daily quest to finish.
	QuestSystem::Instance->WaitForDailyQuest(DailyQuests::GraphicsSystemUpdateDynamicUniformData);

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(commandBuffers[currentCommandBuffer], waitSemaphores, waitStages, signalSemaphores);
}

/*
*	Updates the dynamic uniform data.
*/
void GraphicsSystem::UpdateDynamicUniformData() CATALYST_NOEXCEPT
{
	//Calculate the camera data
	Vector3 cameraWorldPosition = activeCamera->GetWorldPosition();
	Vector3 forwardVector = activeCamera->GetForwardVector();
	Vector3 upVector = activeCamera->GetUpVector();

	Matrix4 cameraMatrix = Matrix4::LookAt(cameraWorldPosition, cameraWorldPosition + forwardVector, upVector);
	viewMatrix.Set(projectionMatrix * cameraMatrix);

	Matrix4 inverseCameraMatrix{ cameraMatrix };
	inverseCameraMatrix.Inverse();

	Matrix4 cameraOriginMatrix{ cameraMatrix };
	cameraOriginMatrix.SetTranslation(Vector3(0.0f, 0.0f, 0.0f));

	Matrix4 inverseProjectionMatrix{ projectionMatrix };
	inverseProjectionMatrix.Inverse();

	dynamicUniformData.inverseCameraMatrix = inverseCameraMatrix;
	dynamicUniformData.inverseProjectionMatrix = inverseProjectionMatrix;
	dynamicUniformData.originViewMatrix = projectionMatrix * cameraOriginMatrix;
	dynamicUniformData.viewMatrix = viewMatrix.GetUnsafe();
	dynamicUniformData.cameraWorldPosition = cameraWorldPosition;

	size_t counter = 0;

	dynamicUniformData.numberOfPointLights = static_cast<uint64>(PointLightEntity::pointLightEntities.Size());

	for (size_t i = 0; i < static_cast<uint64>(PointLightEntity::pointLightEntities.Size()); ++i)
	{
		if (!PointLightEntity::pointLightEntities[i]->GetEnabled())
		{
			--dynamicUniformData.numberOfPointLights;

			continue;
		}

		dynamicUniformData.pointLightAttenuationDistances[counter] = PointLightEntity::pointLightEntities[counter]->GetAttenuationDistance();
		dynamicUniformData.pointLightIntensities[counter] = PointLightEntity::pointLightEntities[counter]->GetIntensity();
		dynamicUniformData.pointLightColors[i] = PointLightEntity::pointLightEntities[counter]->GetLightColor();
		dynamicUniformData.pointLightWorldPositions[counter] = PointLightEntity::pointLightEntities[counter]->GetWorldPosition();

		++counter;
	}

	counter = 0;

	dynamicUniformData.numberOfSpotLights = static_cast<uint64>(SpotLightEntity::spotLightEntities.Size());

	for (size_t i = 0; i < static_cast<uint64>(SpotLightEntity::spotLightEntities.Size()); ++i)
	{
		if (!SpotLightEntity::spotLightEntities[i]->GetEnabled())
		{
			--dynamicUniformData.numberOfSpotLights;

			continue;
		}

		dynamicUniformData.spotLightAttenuationDistances[counter] = SpotLightEntity::spotLightEntities[i]->GetAttenuationDistance();
		dynamicUniformData.spotLightIntensities[counter] = SpotLightEntity::spotLightEntities[i]->GetIntensity();
		dynamicUniformData.spotLightInnerCutoffAngles[counter] = GameMath::CosineDegrees(SpotLightEntity::spotLightEntities[i]->GetInnerCutoffAngle());
		dynamicUniformData.spotLightOuterCutoffAngles[counter] = GameMath::CosineDegrees(SpotLightEntity::spotLightEntities[i]->GetOuterCutoffAngle());
		dynamicUniformData.spotLightColors[counter] = SpotLightEntity::spotLightEntities[i]->GetLightColor();
		dynamicUniformData.spotLightDirections[counter] = SpotLightEntity::spotLightEntities[i]->GetDirection();
		dynamicUniformData.spotLightWorldPositions[counter] = SpotLightEntity::spotLightEntities[i]->GetWorldPosition();

		++counter;
	}

	//Upload the dynamic uniform data to the uniform buffer.
	uniformBuffers[UniformBuffer::DynamicUniformDataBuffer]->UploadData(static_cast<void*>(&dynamicUniformData));
}

/*
*	Updates the view frustum culling.
*/
void GraphicsSystem::UpdateViewFrustumCulling() CATALYST_NOEXCEPT
{
	//Make a local copies of all values that will be worked with.
	const Matrix4 localViewMatrix = viewMatrix.GetSafe();

	//Iterate over all physical entities to check if they are in the view frustum.
	for (PhysicalEntity * CATALYST_RESTRICT physicalEntity : PhysicalEntity::physicalEntities)
	{
		//Make a local copy of the physical entity's position.
		const Vector3 position = physicalEntity->GetWorldPosition();
		const Vector3 scale = physicalEntity->GetWorldScale();
		const float extent = physicalEntity->GetModel().GetExtent();
		const float biggestScale = GameMath::Maximum(scale.X, GameMath::Maximum(scale.Y, scale.Z));
		const float scaledExtent = extent * biggestScale;

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

			corners[i] = localViewMatrix * corners[i];

			corners[i].X /= corners[i].W;
			corners[i].Y /= corners[i].W;
			corners[i].Z /= corners[i].W;
		}

		physicalEntity->SetIsInViewFrustum(GraphicsUtilities::IsCubeWithinViewFrustum(corners));
	}
}