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
*	Pre-initializes the graphics system.
*/
void GraphicsSystem::PreInitializeSystem() CATALYST_NOEXCEPT
{
	//Initialize the main window.
	mainWindow.Initialize();

	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize(mainWindow);

	//Resize the number of command buffers to be equal to the amount of swapchain images.
	commandBuffers.Resize(VulkanInterface::Instance->GetSwapchain().GetSwapChainImages().Size());

	//Allocate all command buffers.
	for (auto &commandBuffer : commandBuffers)
	{
		VulkanInterface::Instance->GetGraphicsCommandPool().AllocateVulkanCommandBuffer(commandBuffer);
	}

	//Initialize all shader modules.
	InitializeShaderModules();

	//Initialize all pipelines.
	InitializePipelines();

	//Initialize all semaphores.
	semaphores[Semaphore::ImageAvailable] = VulkanInterface::Instance->CreateSemaphore();
	semaphores[Semaphore::RenderFinished] = VulkanInterface::Instance->CreateSemaphore();

	//Create the dynamic uniform data buffer.
	uniformBuffers[UniformBuffer::DynamicUniformDataBuffer] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(DynamicUniformData));

	//Create all default textures.
	const byte defaultRoughness[]{ 255 };
	defaultTextures[DefaultTexture::Roughness] = VulkanInterface::Instance->CreateTexture(1, 1, defaultRoughness);

	const byte defaultMetallic[]{ 0 };
	defaultTextures[DefaultTexture::Metallic] = VulkanInterface::Instance->CreateTexture(1, 1, defaultMetallic);

	const byte defaultAmbientOcclusion[]{ 255 };
	defaultTextures[DefaultTexture::AmbientOcclusion] = VulkanInterface::Instance->CreateTexture(1, 1, defaultAmbientOcclusion);
}

/*
*	Post-initializes the graphics system.
*/
void GraphicsSystem::PostInitializeSystem() CATALYST_NOEXCEPT
{
	//Register the graphics system asynchronous update daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::GraphicsSystemAsynchronousUpdate, [](void *CATALYST_RESTRICT) { GraphicsSystem::Instance->UpdateSystemAsynchronous(); }, nullptr);
}

/*
*	Updates the graphics system synchronously.
*/
void GraphicsSystem::UpdateSystemSynchronous() CATALYST_NOEXCEPT
{
	//Carry out the graphics system asynchronous update daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::GraphicsSystemAsynchronousUpdate);

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

	//Update the dynamic uniform data.
	UpdateDynamicUniformData();

	//Render all physical entities.
	RenderPhysicalEntities();

	//End the frame.
	EndFrame();

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(semaphores[Semaphore::RenderFinished]);
}

/*
*	Updates the graphics system asynchronously.
*/
void GraphicsSystem::UpdateSystemAsynchronous() CATALYST_NOEXCEPT
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
void GraphicsSystem::CreatePhysicalDescriptorSet(VulkanDescriptorSet &vulkanDescriptorSet, const VulkanUniformBuffer *CATALYST_RESTRICT modelDataUniformBuffer, const VulkanTexture *CATALYST_RESTRICT albedoTexture, const VulkanTexture *CATALYST_RESTRICT normalMapTexture, const VulkanTexture *CATALYST_RESTRICT roughnessTexture, const VulkanTexture *CATALYST_RESTRICT metallicTexture, const VulkanTexture *CATALYST_RESTRICT ambientOcclusionTexture) const CATALYST_NOEXCEPT
{
	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(vulkanDescriptorSet, pipelines[Pipeline::PhysicalPipeline]->GetDescriptorSetLayout());

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
const PhysicalModel GraphicsSystem::CreatePhysicalModel(const char *CATALYST_RESTRICT modelPath, VulkanTexture *CATALYST_RESTRICT albedoTexture, VulkanTexture *CATALYST_RESTRICT normalMapTexture, VulkanTexture *CATALYST_RESTRICT roughnessTexture, VulkanTexture *CATALYST_RESTRICT metallicTexture, VulkanTexture *CATALYST_RESTRICT ambientOcclusionTexture) const CATALYST_NOEXCEPT
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
*	Creates a texture and returns the identifier for the texture.
*/
VulkanTexture* GraphicsSystem::CreateTexture(const char *CATALYST_RESTRICT texturePath) const CATALYST_NOEXCEPT
{
	//Load the texture.
	int width = 0;
	int height = 0;
	int numberOfChannels = 0;
	byte *textureData = nullptr;

	TextureLoader::LoadTexture(texturePath, width, height, numberOfChannels, &textureData);

	//Create the Vulkan texture.
	VulkanTexture *CATALYST_RESTRICT newTexture = VulkanInterface::Instance->CreateTexture(static_cast<uint32>(width), static_cast<uint32>(height), textureData);

	//Free the texture.
	TextureLoader::FreeTexture(textureData);

	//Return the texture.
	return newTexture;
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
*	Initializes all shader modules.
*/
void GraphicsSystem::InitializeShaderModules() CATALYST_NOEXCEPT
{
	//Initialize the physical vertex shader module.
	const auto physicalVertexShaderByteCode = ShaderLoader::LoadShader("PhysicalVertexShader.spv");
	shaderModules[ShaderModule::PhysicalVertexShaderModule] = VulkanInterface::Instance->CreateShaderModule(physicalVertexShaderByteCode, VK_SHADER_STAGE_VERTEX_BIT);

	//Initialize the physical fragment shader module.
	const auto physicalFragmentShaderByteCode = ShaderLoader::LoadShader("PhysicalFragmentShader.spv");
	shaderModules[ShaderModule::PhysicalFragmentShaderModule] = VulkanInterface::Instance->CreateShaderModule(physicalFragmentShaderByteCode, VK_SHADER_STAGE_FRAGMENT_BIT);
}

/*
*	Initializes all pipelines.
*/
void GraphicsSystem::InitializePipelines() CATALYST_NOEXCEPT
{
	//Create the physical pipeline.
	VulkanPipelineCreationParameters physicalPipelineCreationParameters;

	physicalPipelineCreationParameters.shaderModules.Reserve(2);
	physicalPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::PhysicalVertexShaderModule]);
	physicalPipelineCreationParameters.shaderModules.EmplaceUnsafe(shaderModules[ShaderModule::PhysicalFragmentShaderModule]);
	physicalPipelineCreationParameters.viewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
	physicalPipelineCreationParameters.descriptorLayoutBindingInformations.Reserve(7);
	physicalPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	physicalPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	physicalPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	physicalPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	physicalPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	physicalPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	physicalPipelineCreationParameters.descriptorLayoutBindingInformations.EmplaceUnsafe(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	physicalPipelineCreationParameters.colorAttachmens = VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews();
	physicalPipelineCreationParameters.depthBuffer = &VulkanInterface::Instance->GetSwapchain().GetDepthBuffer();

	pipelines[Pipeline::PhysicalPipeline] = VulkanInterface::Instance->CreatePipeline(physicalPipelineCreationParameters);
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

	//Set the current command buffer.
	currentCommandBuffer = VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();

	//Set up the current command buffer.
	commandBuffers[currentCommandBuffer].Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	commandBuffers[currentCommandBuffer].CommandBindPipeline(*pipelines[Pipeline::PhysicalPipeline]);
	commandBuffers[currentCommandBuffer].CommandBeginRenderPass(pipelines[Pipeline::PhysicalPipeline]->GetRenderPass(), VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());
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
*	Renders all physical entities.
*/
void GraphicsSystem::RenderPhysicalEntities() CATALYST_NOEXCEPT
{
	//Iterate over all physical entities and draw them all.
	for (PhysicalEntity * CATALYST_RESTRICT physicalEntity : PhysicalEntity::physicalEntities)
	{
		//Update this physical entity's model matrix.
		physicalEntity->UpdateModelMatrix();

		//Don't draw this physical entity if it isn't in the view frustum.
		if (!physicalEntity->IsInViewFrustum())
			continue;

		commandBuffers[currentCommandBuffer].CommandBindDescriptorSets(*pipelines[Pipeline::PhysicalPipeline], physicalEntity->GetDescriptorSet());
		commandBuffers[currentCommandBuffer].CommandBindVertexBuffers(*physicalEntity->GetModel().GetVertexBuffer());
		commandBuffers[currentCommandBuffer].CommandBindIndexBuffer(*physicalEntity->GetModel().GetIndexBuffer());
		commandBuffers[currentCommandBuffer].CommandDrawIndexed(physicalEntity->GetModel().GetIndexCount());
	}
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
	commandBuffers[currentCommandBuffer].CommandEndRenderPass();
	commandBuffers[currentCommandBuffer].End();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue().Submit(commandBuffers[currentCommandBuffer], waitSemaphores, waitStages, signalSemaphores);
}