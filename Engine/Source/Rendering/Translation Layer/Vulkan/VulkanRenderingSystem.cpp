//Header file.
#include <Rendering/Translation Layer/Vulkan/VulkanRenderingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/InstancedPhysicalEntity.h>
#include <Entities/ParticleSystemEntity.h>
#include <Entities/PointLightEntity.h>
#include <Entities/SpotLightEntity.h>
#include <Entities/StaticPhysicalEntity.h>
#include <Entities/TerrainEntity.h>
#include <Entities/VegetationEntity.h>
#include <Entities/Initialization Data/TerrainInitializationData.h>

//Managers.
#include <Managers/EnvironmentManager.h>
#include <Managers/PostProcessingManager.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Matrix3.h>

//Multithreading.
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture2D.h>
#include <Rendering/Engine Layer/ParticleMaterial.h>
#include <Rendering/Engine Layer/PostProcessingUniformData.h>
#include <Rendering/Engine Layer/RenderingUtilities.h>
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/TextureData.h>
#include <Rendering/Engine Layer/VegetationMaterial.h>
#include <Rendering/Engine Layer/Render Passes/RenderPasses.h>
#include <Rendering/Shader Data/Vulkan/VulkanShaderData.h>
#include <Rendering/Translation Layer/Vulkan/VulkanParticleSystemProperties.h>
#include <Rendering/Translation Layer/Vulkan/VulkanTranslationCommandBuffer.h>
#include <Rendering/Translation Layer/Vulkan/VulkanTranslationUtilities.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanRenderingSystem);

//Vulkan rendering system constants.
namespace VulkanRenderingSystemConstants
{
	constexpr VkDeviceSize PARTICLE_SYSTEM_STORAGE_BUFFER_SIZE{ 1'310'736 };
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

	//Initialize all special textures.
	InitializeSpecialTextures();

	//Initialize all render targets.
	InitializeRenderTargets();

	//Initialize all semaphores.
	InitializeSemaphores();

	//Initialize all uniform buffers.
	InitializeUniformBuffers();

	//Initialize all descriptor set layouts.
	InitializeDescriptorSetLayouts();

	//Initialize all shader modules.
	InitializeShaderModules();

	//Initialize all descriptor sets.
	InitializeDescriptorSets();

	//Initialize the Vulkan frame data.
	frameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages(), descriptorSetLayouts[INDEX(RenderDataTableLayout::DynamicUniformData)], descriptorSetLayouts[INDEX(RenderDataTableLayout::Environment)], descriptorSetLayouts[INDEX(RenderDataTableLayout::Ocean)]);
}

/*
*	Pre-updates the Vulkan rendering system synchronously.
*/
void VulkanRenderingSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Execute the asynchronous tasks.
	ExecuteAsynchronousTasks();

	//Update the post processing data.
	UpdatePostProcessingData();

	//Update the main window.
	mainWindow.Update();

	//Pre-update the Vulkan interface.
	VulkanInterface::Instance->PreUpdate(semaphores[INDEX(GraphicsSemaphore::ImageAvailable)]);

	//Begin the frame.
	BeginFrame();

	//Update the dynamic uniform data.
	UpdateDynamicUniformData();

	//Update the descriptor sets.
	UpdateDescriptorSets();
}

/*
*	Post-updates the Vulkan rendering system synchronously.
*/
void VulkanRenderingSystem::PostUpdateSystemSynchronous() NOEXCEPT
{
	//Concatenate all secondary command buffers into the previous one.
	ConcatenateCommandBuffers();

	//End the frame.
	EndFrame();

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(semaphores[INDEX(GraphicsSemaphore::RenderFinished)]);

	//Check if the main window should close - If so, terminate.
	if (mainWindow.ShouldClose())
	{
		EngineSystem::Instance->Terminate();
	}
}

/*
*	Releases the Vulkan rendering system.
*/
void VulkanRenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release all descriptor set layouts.
	for (uint32 i = 0; i < INDEX(RenderDataTableLayout::NumberOfDescriptorSetLayouts); ++i)
	{
		descriptorSetLayouts[i].Release();
	}

	//Release the main window.
	mainWindow.Release();

	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Returns the resolution.
*/
Resolution VulkanRenderingSystem::GetResolution() const NOEXCEPT
{
	//Return the render resolution.
	VkExtent2D swapExtent{ VulkanInterface::Instance->GetSwapchain().GetSwapExtent() };

	return Resolution(swapExtent.width, swapExtent.height);
}

/*
*	Returns the current frame index.
*/
uint8 VulkanRenderingSystem::GetCurrentFrameIndex() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();
}

/*
*	Creates a constant buffer.
*/
ConstantBufferHandle VulkanRenderingSystem::CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT
{
	return reinterpret_cast<ConstantBufferHandle>(VulkanInterface::Instance->CreateConstantBuffer(data, dataSizes, dataChunks)->Get());
}

/*
*	Destroys a render data table.
*/
void VulkanRenderingSystem::DestroyRenderDataTable(RenderDataTableHandle renderDataTable) const NOEXCEPT
{
	VulkanInterface::Instance->DestroyDescriptorSet(reinterpret_cast<VkDescriptorSet>(renderDataTable));
}

/*
*	Creates an environment material.
*/
void VulkanRenderingSystem::CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT
{
	//Create the diffuse texture.
	environmentMaterial.diffuseTexture = static_cast<TextureCubeMapHandle>(VulkanInterface::Instance->CreateCubeMapTexture(environmentMaterialData.diffuseData.Data(), environmentMaterialData.diffuseResolution, environmentMaterialData.diffuseResolution));

	//Create the diffuse irradiance texture.
	environmentMaterial.diffuseIrradianceTexture = static_cast<TextureCubeMapHandle>(VulkanInterface::Instance->CreateCubeMapTexture(environmentMaterialData.diffuseIrradianceData.Data(), environmentMaterialData.diffuseIrradianceResolution, environmentMaterialData.diffuseIrradianceResolution));
}

/*
*	Initializes a terrain entity.
*/
void VulkanRenderingSystem::InitializeTerrainEntity(const TerrainEntity *const RESTRICT entity, const TerrainInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Fill the terrain entity components with the relevant data.
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetTerrainFrustumCullingComponents()[entity->GetComponentsIndex()] };
	TerrainComponent &terrainComponent{ ComponentManager::GetTerrainComponents()[entity->GetComponentsIndex()] };
	TerrainRenderComponent &terrainRenderComponent{ ComponentManager::GetTerrainRenderComponents()[entity->GetComponentsIndex()] };

	frustumCullingComponent.axisAlignedBoundingBox = data->axisAlignedBoundingBox;

	Texture2DHandle terrainPropertiesTexture = Create2DTexture(TextureData(TextureDataContainer(data->terrainProperties), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Nearest, TextureFormat::R32G32B32A32_Float));

	terrainComponent.terrainUniformData = data->terrainUniformData;
	terrainComponent.uniformBuffer = VulkanInterface::Instance->CreateUniformBuffer(sizeof(TerrainUniformData));
	static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent.uniformBuffer)->UploadData(&terrainComponent.terrainUniformData);
 	terrainComponent.terrainProperties = data->terrainProperties;
	terrainComponent.terrainPropertiesTexture = terrainPropertiesTexture;

	//Create the descriptor set.
	VulkanDescriptorSet newDescriptorSet;
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, descriptorSetLayouts[INDEX(RenderDataTableLayout::Terrain)]);

	terrainRenderComponent.renderDataTable = reinterpret_cast<RenderDataTableHandle>(newDescriptorSet.Get());

	DynamicArray<VkWriteDescriptorSet, 18> writeDescriptorSets;

	writeDescriptorSets.EmplaceFast(static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent.uniformBuffer)->GetWriteDescriptorSet(newDescriptorSet, 1));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(terrainPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 2));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->layerWeightsTexture)->GetWriteDescriptorSet(newDescriptorSet, 3));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.firstLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 4));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.firstLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 5));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.firstLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 6));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.secondLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 7));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.secondLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 8));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.secondLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 9));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.thirdLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 10));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.thirdLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 11));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.thirdLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 12));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.fourthLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 13));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.fourthLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 14));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.fourthLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 15));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.fifthLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 16));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.fifthLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 17));
	writeDescriptorSets.EmplaceFast(static_cast<const Vulkan2DTexture *RESTRICT>(data->terrainMaterial.fifthLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 18));

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
}

/*
*	Initializes a static physical entity.
*/
void VulkanRenderingSystem::InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT
{
	//Cache relevant data.
	VulkanDescriptorSet newDescriptorSet;
	const PhysicalMaterial &material = model.GetMaterial();

	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, descriptorSetLayouts[INDEX(RenderDataTableLayout::Physical)]);

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 3> writeDescriptorSets
	{
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.normalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.materialPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 3)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Fill the static physical entity components with the relevant data.
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetStaticPhysicalFrustumCullingComponents()[staticPhysicalEntity.GetComponentsIndex()] };
	StaticPhysicalRenderComponent &renderComponent{ ComponentManager::GetStaticPhysicalRenderComponents()[staticPhysicalEntity.GetComponentsIndex()] };
	TransformComponent &transformComponent{ ComponentManager::GetStaticPhysicalTransformComponents()[staticPhysicalEntity.GetComponentsIndex()] };

	frustumCullingComponent.axisAlignedBoundingBox = model.GetAxisAlignedBoundingBox();
	renderComponent.modelMatrix = Matrix4(position, rotation, scale);
	renderComponent.renderDataTable = reinterpret_cast<RenderDataTableHandle>(newDescriptorSet.Get());
	renderComponent.buffer = model.GetBuffer();
	renderComponent.indexOffset = model.GetIndexOffset();
	renderComponent.indexCount = model.GetIndexCount();
	transformComponent.position = position;
	transformComponent.rotation = rotation;
	transformComponent.scale = scale;
}

/*
*	Initializes an instanced physical entity.
*/
void VulkanRenderingSystem::InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Cache relevant data.
	VulkanDescriptorSet newDescriptorSet;
	const PhysicalMaterial &material = model.GetMaterial();

	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, descriptorSetLayouts[INDEX(RenderDataTableLayout::Physical)]);

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 3> writeDescriptorSets
	{
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.normalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.materialPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 3)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Create the transformations buffer.
	const void *RESTRICT transformationsData[]{ transformations.Data() };
	const VkDeviceSize transformationsDataSizes[]{ sizeof(Matrix4) * transformations.Size() };
	VulkanConstantBuffer *RESTRICT transformationsBuffer = VulkanInterface::Instance->CreateConstantBuffer(transformationsData, transformationsDataSizes, 1);

	//Fill the instanced physical entity components with the relevant data.
	InstancedPhysicalRenderComponent &renderComponent{ ComponentManager::GetInstancedPhysicalRenderComponents()[entity.GetComponentsIndex()] };

	renderComponent.renderDataTable = reinterpret_cast<RenderDataTableHandle>(newDescriptorSet.Get());
	renderComponent.modelBuffer = model.GetBuffer();
	renderComponent.transformationsBuffer = reinterpret_cast<ConstantBufferHandle>(transformationsBuffer->Get());
	renderComponent.indexOffset = model.GetIndexOffset();
	renderComponent.indexCount = model.GetIndexCount();
	renderComponent.instanceCount = static_cast<uint32>(transformations.Size());
}

/*
*	Initializes a vegetation entity.
*/
void VulkanRenderingSystem::InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT
{
	//Get the components.
	VegetationComponent &renderComponent{ ComponentManager::GetVegetationComponents()[entity.GetComponentsIndex()] };
	VegetationCullingComponent &cullingComponent{ ComponentManager::GetVegetationCullingComponents()[entity.GetComponentsIndex()] };

	//Calculate the vegetation grid.
	DynamicArray<VegetationTransformation> sortedTransformations;
	RenderingUtilities::CalculateVegetationGrid(properties.cutoffDistance, transformations, &renderComponent, &cullingComponent, sortedTransformations);

	//The cutoff distance is used as a squared value in the shadres, so square it.
	VegetationProperties shaderProperties{ properties };
	shaderProperties.cutoffDistance *= shaderProperties.cutoffDistance;

	//Create the vegetation properties uniform buffer.
	VulkanUniformBuffer *const RESTRICT propertiesBuffer{ VulkanInterface::Instance->CreateUniformBuffer(sizeof(VegetationProperties)) };
	propertiesBuffer->UploadData(&shaderProperties);

	//Create the descriptor set.
	VulkanDescriptorSet newDescriptorSet;

	//Allocate the descriptor set.
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(newDescriptorSet, descriptorSetLayouts[INDEX(RenderDataTableLayout::Vegetation)]);

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 5> writeDescriptorSets
	{
		propertiesBuffer->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.maskTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 3),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.normalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 4),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.propertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 5)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Create the transformations buffer.
	const void *RESTRICT transformationsData[]{ sortedTransformations.Data() };
	const VkDeviceSize transformationsDataSizes[]{ sizeof(VegetationTransformation) * sortedTransformations.Size() };
	VulkanConstantBuffer *RESTRICT transformationsBuffer = VulkanInterface::Instance->CreateConstantBuffer(transformationsData, transformationsDataSizes, 1);

	//Fill the components with the relevant data.
	renderComponent.renderDataTable = reinterpret_cast<RenderDataTableHandle>(newDescriptorSet.Get());
	renderComponent.transformationsBuffer = reinterpret_cast<ConstantBufferHandle>(transformationsBuffer->Get());
}

/*
*	Initializes a particle system entity.
*/
void VulkanRenderingSystem::InitializeParticleSystemEntity(const ParticleSystemEntity &entity, const ParticleMaterial &material, const ParticleSystemProperties &properties) const NOEXCEPT
{
	//Create the storage buffer.
	VulkanStorageBuffer *const RESTRICT storageBuffer{ VulkanInterface::Instance->CreateStorageBuffer(VulkanRenderingSystemConstants::PARTICLE_SYSTEM_STORAGE_BUFFER_SIZE) };

	//Create the uniform buffer.
	VulkanUniformBuffer *const RESTRICT uniformBuffer{ VulkanInterface::Instance->CreateUniformBuffer(static_cast<VkDeviceSize>(sizeof(VulkanParticleSystemProperties))) };
	const VulkanParticleSystemProperties vulkanParticleSystemProperties{ properties };
	uniformBuffer->UploadData(&vulkanParticleSystemProperties);

	//Create the descriptor set.
	VulkanDescriptorSet renderDataTable;
	VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(renderDataTable, descriptorSetLayouts[INDEX(RenderDataTableLayout::ParticleSystem)]);

	StaticArray<VkWriteDescriptorSet, 3> particleSystemWriteDescriptorSets
	{
		storageBuffer->GetWriteDescriptorSet(renderDataTable, 1),
		uniformBuffer->GetWriteDescriptorSet(renderDataTable, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(renderDataTable, 3)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(particleSystemWriteDescriptorSets.Size()), particleSystemWriteDescriptorSets.Data(), 0, nullptr);

	//Set up the particle system's components.
	ParticleSystemComponent &component{ ComponentManager::GetParticleSystemComponents()[entity.GetComponentsIndex()] };
	ParticleSystemRenderComponent &renderComponent{ ComponentManager::GetParticleSystemRenderComponents()[entity.GetComponentsIndex()] };

	component.properties = properties;
	component.propertiesUniformBuffer = uniformBuffer;
	renderComponent.renderDataTable = reinterpret_cast<RenderDataTableHandle>(renderDataTable.Get());
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
*	Destroys a texture 2D
*/
void VulkanRenderingSystem::DestroyTexture2D(Texture2DHandle texture) const NOEXCEPT
{
	//Destroy the texture.
	VulkanInterface::Instance->Destroy2DTexture(static_cast<Vulkan2DTexture *const RESTRICT>(texture));
}

/*
*	Creates a uniform buffer and returns the identifier for the uniform buffer.
*/
UniformBufferHandle VulkanRenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize) const NOEXCEPT
{
	return VulkanInterface::Instance->CreateUniformBuffer(uniformBufferSize);
}

/*
*	Finalizes the initialization of a render pass.
*/
void VulkanRenderingSystem::FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Create the pipeline.
	VulkanPipelineCreationParameters parameters;

	parameters.attachmentLoadOperator = VulkanTranslationUtilities::GetVulkanAttachmentLoadOperator(renderPass->GetColorAttachmentLoadOperator());
	parameters.blendEnable = renderPass->IsBlendEnabled();

	if (renderPass->GetRenderTargets()[0] == RenderTarget::Screen)
	{
		parameters.colorAttachmentFinalLayout = VULKAN_IMAGE_LAYOUT_PRESENT_SRC;
		parameters.colorAttachmentFormat = VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format;
		parameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		const DynamicArray<VkImageView> &swapchainImageViews{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
		const uint64 swapchainImageViewsSize{ swapchainImageViews.Size() };

		parameters.colorAttachments.UpsizeSlow(swapchainImageViewsSize);

		for (uint64 i = 0; i < swapchainImageViewsSize; ++i)
		{
			parameters.colorAttachments[i].Reserve(1);
			parameters.colorAttachments[i].EmplaceFast(swapchainImageViews[i]);
		}
	}

	else
	{
		parameters.colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		parameters.colorAttachmentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		parameters.colorAttachmentInitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		parameters.colorAttachments.UpsizeSlow(1);
		parameters.colorAttachments[0].Reserve(renderPass->GetRenderTargets().Size());

		for (const RenderTarget renderTarget : renderPass->GetRenderTargets())
		{
			parameters.colorAttachments[0].EmplaceFast(renderTargets[INDEX(renderTarget)]->GetImageView());
		}
	}

	parameters.cullMode = VulkanTranslationUtilities::GetVulkanCullMode(renderPass->GetCullMode());
	parameters.depthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	parameters.depthAttachmentInitialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	parameters.depthAttachmentStoreOp = VulkanTranslationUtilities::GetVulkanAttachmentStoreOperator(renderPass->GetDepthAttachmentStoreOperator());
	parameters.depthBuffer = renderPass->GetDepthBuffer() == DepthBuffer::None ? nullptr : depthBuffers[INDEX(renderPass->GetDepthBuffer())];
	parameters.depthCompareOp = VulkanTranslationUtilities::GetVulkanCompareOperator(renderPass->GetDepthCompareOperator());
	parameters.depthTestEnable = renderPass->IsDepthTestEnabled();
	parameters.depthWriteEnable = renderPass->IsDepthWriteEnabled();
	
	DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
	pipelineDescriptorSetLayouts.Reserve(renderPass->GetDescriptorSetLayouts().Size());

	for (const RenderDataTableLayout descriptorSetLayout : renderPass->GetDescriptorSetLayouts())
	{
		pipelineDescriptorSetLayouts.EmplaceFast(descriptorSetLayouts[INDEX(descriptorSetLayout)]);
	}

	parameters.descriptorSetLayoutCount = static_cast<uint32>(pipelineDescriptorSetLayouts.Size());
	parameters.descriptorSetLayouts = pipelineDescriptorSetLayouts.Data();

	DynamicArray<VkPushConstantRange> pushConstantRanges;

	if (renderPass->GetPushConstantRanges().Empty())
	{
		parameters.pushConstantRangeCount = 0;
		parameters.pushConstantRanges = nullptr;
	}

	else
	{
		pushConstantRanges.Reserve(renderPass->GetPushConstantRanges().Size());

		for (const PushConstantRange &pushConstantRange : renderPass->GetPushConstantRanges())
		{
			pushConstantRanges.EmplaceFast(VulkanTranslationUtilities::GetVulkanPushConstantRange(pushConstantRange));
		}

		parameters.pushConstantRangeCount = static_cast<uint32>(pushConstantRanges.Size());
		parameters.pushConstantRanges = pushConstantRanges.Data();
	}
	
	parameters.shaderModules.Reserve(5);
	if (renderPass->GetVertexShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetVertexShader())]);
	if (renderPass->GetTessellationControlShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetTessellationControlShader())]);
	if (renderPass->GetTessellationEvaluationShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetTessellationEvaluationShader())]);
	if (renderPass->GetGeometryShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetGeometryShader())]);
	if (renderPass->GetFragmentShader() != Shader::None) parameters.shaderModules.EmplaceFast(shaderModules[INDEX(renderPass->GetFragmentShader())]);
	parameters.topology = VulkanTranslationUtilities::GetVulkanTopology(renderPass->GetTopology());

	DynamicArray<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	vertexInputAttributeDescriptions.Reserve(renderPass->GetVertexInputAttributeDescriptions().Size());

	for (const VertexInputAttributeDescription &vertexInputAttributeDescription : renderPass->GetVertexInputAttributeDescriptions())
	{
		vertexInputAttributeDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputAttributeDescription(vertexInputAttributeDescription));
	}

	parameters.vertexInputAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.Size());
	parameters.vertexInputAttributeDescriptions = vertexInputAttributeDescriptions.Data();

	DynamicArray<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	vertexInputBindingDescriptions.Reserve(renderPass->GetVertexInputBindingDescriptions().Size());

	for (const VertexInputBindingDescription &vertexInputBindingDescription : renderPass->GetVertexInputBindingDescriptions())
	{
		vertexInputBindingDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputBindingDescription(vertexInputBindingDescription));
	}

	parameters.vertexInputBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.Size());
	parameters.vertexInputBindingDescriptions = vertexInputBindingDescriptions.Data();
	parameters.viewportExtent = VkExtent2D{ renderPass->GetRenderResolution().width, renderPass->GetRenderResolution().height };

	//Create the pipeline!
	pipelines[INDEX(renderPass->GetStage())] = VulkanInterface::Instance->CreatePipeline(parameters);

	//Update the Vulkan render pass data.
	vulkanRenderPassData[INDEX(renderPass->GetStage())].framebuffers.Reserve(pipelines[INDEX(renderPass->GetStage())]->GetRenderPass().GetFrameBuffers().Size());

	for (const VulkanFramebuffer& framebuffer : pipelines[INDEX(renderPass->GetStage())]->GetRenderPass().GetFrameBuffers())
	{
		vulkanRenderPassData[INDEX(renderPass->GetStage())].framebuffers.EmplaceFast(framebuffer.Get());
	}

	vulkanRenderPassData[INDEX(renderPass->GetStage())].pipeline = pipelines[INDEX(renderPass->GetStage())]->Get();
	vulkanRenderPassData[INDEX(renderPass->GetStage())].pipelineLayout = pipelines[INDEX(renderPass->GetStage())]->GetPipelineLayout();
	vulkanRenderPassData[INDEX(renderPass->GetStage())].renderPass = pipelines[INDEX(renderPass->GetStage())]->GetRenderPass().Get();

	renderPass->SetData(&vulkanRenderPassData[INDEX(renderPass->GetStage())]);

	//Add the command buffers.
	const uint64 numberOfCommandBuffers{ VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() };
	renderPass->SetNumberOfCommandBuffers(numberOfCommandBuffers);

	//Create the directional shadow command pool.
	VulkanCommandPool *const RESTRICT pipelineCommandPool{ VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) };

	for (uint64 i = 0; i < numberOfCommandBuffers; ++i)
	{
		VulkanCommandBuffer pipelineCommandBuffer;
		pipelineCommandPool->AllocateSecondaryCommandBuffer(pipelineCommandBuffer);
		renderPass->AddCommandBuffer(new VulkanTranslationCommandBuffer(pipelineCommandBuffer));
	}
}

/*
*	Returns the current dynamic uniform data descriptor set.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCurrentDynamicUniformDataDescriptorSet() NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(frameData.GetCurrentDynamicUniformDataDescriptorSet()->Get());
}

/*
*	Returns the current environment data descriptor set.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCurrentEnvironmentDataDescriptorSet() NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(frameData.GetCurrentEnvironmentDescriptorSet()->Get());
}

/*
*	Returns the current ocean descriptor set.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCurrentOceanDescriptorSet() NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(frameData.GetCurrentOceanDescriptorSet()->Get());
}

/*
*	Returns the given render data table.
*/
RenderDataTableHandle VulkanRenderingSystem::GetRenderDataTable(const RenderDataTable renderDataTable) NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(descriptorSets[INDEX(renderDataTable)].Get());
}

/*
*	Initializes all special textures.
*/
void VulkanRenderingSystem::InitializeSpecialTextures() NOEXCEPT
{
	{
		//Initialize the screen space ambient occlusion random kernel texture.
		StaticArray<Vector4, RenderingConstants::SCREEN_SPACE_AMBIENT_OCCLUSION_RANDOM_KERNEL_SIZE> samples;

		for (Vector4& sample : samples)
		{
			sample.X = CatalystMath::RandomFloatInRange(-1.0f, 1.0f);
			sample.Y = CatalystMath::RandomFloatInRange(-1.0f, 1.0f);
			sample.Z = 0.0f;
			sample.W = 0.0f;
		}

		const void* const RESTRICT data{ samples.Data() };

		specialTextures[INDEX(SpecialTexture::ScreenSpaceAmbientOcclusionRandomNoise)] = VulkanInterface::Instance->Create2DTexture(1, 4, 4, 4, sizeof(float), &data, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
	}
}

/*
*	Initializes all render targets.
*/
void VulkanRenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Initialize all depth buffers.
	depthBuffers[INDEX(DepthBuffer::DirectionalLight)] = VulkanInterface::Instance->CreateDepthBuffer({ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION });
	depthBuffers[INDEX(DepthBuffer::SceneBuffer)] = VulkanInterface::Instance->CreateDepthBuffer(VulkanInterface::Instance->GetSwapchain().GetSwapExtent());

	//Initialize all render targets.
	renderTargets[INDEX(RenderTarget::DirectionalShadowMap)] = VulkanInterface::Instance->CreateRenderTarget({ RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION }, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::DirectionalShadow)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::SceneBufferAlbedo)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::ScreenSpaceAmbientOcclusion)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::ScreenSpaceAmbientOcclusionIntermediate)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::SceneIntermediate)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::Scene)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::Bloom)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::BloomIntermediate)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::FogBlurIntermediate)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::FogBlur)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::BlurIntermediate)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::Blur)] = VulkanInterface::Instance->CreateRenderTarget(VulkanInterface::Instance->GetSwapchain().GetSwapExtent(), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
}

/*
*	Initializes all semaphores.
*/
void VulkanRenderingSystem::InitializeSemaphores() NOEXCEPT
{
	//Initialize all semaphores.
	semaphores[INDEX(GraphicsSemaphore::ImageAvailable)] = VulkanInterface::Instance->CreateSemaphore();
	semaphores[INDEX(GraphicsSemaphore::RenderFinished)] = VulkanInterface::Instance->CreateSemaphore();
}

/*
*	Initializes all uniform buffers.
*/
void VulkanRenderingSystem::InitializeUniformBuffers() NOEXCEPT
{
	{
		//Initialize the screen space ambient occlusion sample kernel texture.
		StaticArray<Vector4, RenderingConstants::SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLE_KERNEL_SIZE> samples;

		for (Vector4& sample : samples)
		{
			sample.X = CatalystMath::RandomFloatInRange(-1.0f, 1.0f);
			sample.Y = CatalystMath::RandomFloatInRange(-1.0f, 1.0f);
			sample.Z = CatalystMath::RandomFloatInRange(0.0f, 1.0f);
			sample.W = 0.0f;

			sample.Normalize();

			float scale{ CatalystMath::RandomFloatInRange(0.0f, 1.0f) };
			scale = CatalystMath::LinearlyInterpolate(0.1f, 1.0f, scale * scale);

			sample *= scale;
		}

		uniformBuffers[INDEX(UniformBuffer::ScreenSpaceAmbientOcclusionSamples)] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(Vector4) * RenderingConstants::SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLE_KERNEL_SIZE);
		uniformBuffers[INDEX(UniformBuffer::ScreenSpaceAmbientOcclusionSamples)]->UploadData(samples.Data());
	}

	//Initialize the post processing uniform data buffer.
	uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(PostProcessingUniformData));
}

/*
*	Initializes all descriptor set layouts.
*/
void VulkanRenderingSystem::InitializeDescriptorSetLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> dynamicUniformDataDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::DynamicUniformData)].Initialize(1, dynamicUniformDataDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the shadow map blur descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> shadowMapBlurDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::ShadowMapBlur)].Initialize(static_cast<uint32>(shadowMapBlurDescriptorSetLayoutBindings.Size()), shadowMapBlurDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the environment descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> environmentDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::Environment)].Initialize(static_cast<uint32>(environmentDescriptorSetLayoutBindings.Size()), environmentDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the terrain descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 18> terrainDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(11, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(12, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(13, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(14, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(15, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(16, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(17, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(18, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::Terrain)].Initialize(18, terrainDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the static physical descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> staticPhysicalDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::Physical)].Initialize(3, staticPhysicalDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the vegetation descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 5> vegetationDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::Vegetation)].Initialize(static_cast<uint32>(vegetationDescriptorSetLayoutBindings.Size()), vegetationDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the directional shadow descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 2> descriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::DirectionalShadow)].Initialize(static_cast<uint32>(descriptorSetLayoutBindings.Size()), descriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the screen space ambient occlusion descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 3> descriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::ScreenSpaceAmbientOcclusion)].Initialize(static_cast<uint32>(descriptorSetLayoutBindings.Size()), descriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the lighting descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 5> lightingDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::Lighting)].Initialize(static_cast<uint32>(lightingDescriptorSetLayoutBindings.Size()), lightingDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the ocean descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 3> oceanDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::Ocean)].Initialize(static_cast<uint32>(oceanDescriptorSetLayoutBindings.Size()), oceanDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the particle system descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 3> particleSystemDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::ParticleSystem)].Initialize(static_cast<uint32>(particleSystemDescriptorSetLayoutBindings.Size()), particleSystemDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the post processing bloom descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 2> descriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::PostProcessingBloom)].Initialize(static_cast<uint32>(descriptorSetLayoutBindings.Size()), descriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the bloom descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> descriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)].Initialize(static_cast<uint32>(descriptorSetLayoutBindings.Size()), descriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the post processing fog descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> descriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::PostProcessingFog)].Initialize(static_cast<uint32>(descriptorSetLayoutBindings.Size()), descriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the post processing descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> postProcessingDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(RenderDataTableLayout::PostProcessing)].Initialize(static_cast<uint32>(postProcessingDescriptorSetLayoutBindings.Size()), postProcessingDescriptorSetLayoutBindings.Data());
	}
}

/*
*	Initializes all shader modules.
*/
void VulkanRenderingSystem::InitializeShaderModules() NOEXCEPT
{
	{
		//Initialize the bloom fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBloomFragmentShaderData(data);
		shaderModules[INDEX(Shader::BloomFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the cube map vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetCubeMapVertexShaderData(data);
		shaderModules[INDEX(Shader::CubeMapVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the directional physical shadow vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalPhysicalShadowVertexShaderData(data);
		shaderModules[INDEX(Shader::DirectionalPhysicalShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the directional shadow fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowFragmentShaderData(data);
		shaderModules[INDEX(Shader::DirectionalShadowFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize directional shadow instanced physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowInstancedPhysicalVertexShaderData(data);
		shaderModules[INDEX(Shader::DirectionalInstancedPhysicalShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowTerrainTessellationEvaluationShaderData(data);
		shaderModules[INDEX(Shader::DirectionalTerrainShadowTessellationEvaluation)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetGaussianBlurFragmentShaderData(data);
		shaderModules[INDEX(Shader::GaussianBlurFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the instanced physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetInstancedPhysicalVertexShaderData(data);
		shaderModules[INDEX(Shader::InstancedPhysicalVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the lighting fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLightingFragmentShaderData(data);
		shaderModules[INDEX(Shader::LightingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the ocean fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetOceanFragmentShaderData(data);
		shaderModules[INDEX(Shader::OceanFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the particle system fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemFragmentShaderData(data);
		shaderModules[INDEX(Shader::ParticleSystemFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the particle system geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemGeometryShaderData(data);
		shaderModules[INDEX(Shader::ParticleSystemGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the particle system vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemVertexShaderData(data);
		shaderModules[INDEX(Shader::ParticleSystemVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the post processing bloom fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPostProcessingBloomFragmentShaderData(data);
		shaderModules[INDEX(Shader::PostProcessingBloomFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the post processing fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPostProcessingFragmentShaderData(data);
		shaderModules[INDEX(Shader::PostProcessingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPhysicalFragmentShaderData(data);
		shaderModules[INDEX(Shader::PhysicalFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPhysicalVertexShaderData(data);
		shaderModules[INDEX(Shader::PhysicalVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the screen space ambient occlusion fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetScreenSpaceAmbientOcclusionFragmentShaderData(data);
		shaderModules[INDEX(Shader::ScreenSpaceAmbientOcclusionFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the shadow map fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetShadowMapFragmentShaderData(data);
		shaderModules[INDEX(Shader::ShadowMapFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the sky fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetSkyFragmentShaderData(data);
		shaderModules[INDEX(Shader::SkyFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the terrain scene buffer fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferFragmentShaderData(data);
		shaderModules[INDEX(Shader::TerrainFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the terrain scene buffer tessellation control shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferTessellationControlShaderData(data);
		shaderModules[INDEX(Shader::TerrainTessellationControl)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
	}
	
	{
		//Initialize the terrain scene buffer tessellation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferTessellationEvaluationShaderData(data);
		shaderModules[INDEX(Shader::TerrainTessellationEvaluation)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}
	
	{
		//Initialize the terrain scene buffer vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainVertexShaderData(data);
		shaderModules[INDEX(Shader::TerrainVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationFragmentShaderData(data);
		shaderModules[INDEX(Shader::VegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the vegetation geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationGeometryShaderData(data);
		shaderModules[INDEX(Shader::VegetationGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}
	
	{
		//Initialize the vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationVertexShaderData(data);
		shaderModules[INDEX(Shader::VegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the viewport vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetViewportVertexShaderData(data);
		shaderModules[INDEX(Shader::ViewportVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
}

/*
*	Initializes all descriptor sets.
*/
void VulkanRenderingSystem::InitializeDescriptorSets() NOEXCEPT
{
	{
		//Initialize the directional shadow descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::DirectionalShadow)], descriptorSetLayouts[INDEX(RenderDataTableLayout::DirectionalShadow)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 2> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::DirectionalShadow)], 0),
			renderTargets[INDEX(RenderTarget::DirectionalShadowMap)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::DirectionalShadow)], 1)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the screen space ambient occlusion descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::ScreenSpaceAmbientOcclusion)], descriptorSetLayouts[INDEX(RenderDataTableLayout::ScreenSpaceAmbientOcclusion)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 3> writeDescriptorSets
		{
			uniformBuffers[INDEX(UniformBuffer::ScreenSpaceAmbientOcclusionSamples)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::ScreenSpaceAmbientOcclusion)], 0),
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::ScreenSpaceAmbientOcclusion)], 1),
			specialTextures[INDEX(SpecialTexture::ScreenSpaceAmbientOcclusionRandomNoise)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::ScreenSpaceAmbientOcclusion)], 2)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the screen space ambient occlusion horizontal blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::ScreenSpaceAmbientOcclusionHorizontalBlur)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::ScreenSpaceAmbientOcclusion)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::ScreenSpaceAmbientOcclusionHorizontalBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the screen space ambient occlusion vertical blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::ScreenSpaceAmbientOcclusionVerticalBlur)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::ScreenSpaceAmbientOcclusionIntermediate)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::ScreenSpaceAmbientOcclusionVerticalBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the lighting descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::Lighting)], descriptorSetLayouts[INDEX(RenderDataTableLayout::Lighting)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 5> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::SceneBufferAlbedo)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::Lighting)], 0),
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::Lighting)], 1),
			renderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::Lighting)], 2),
			renderTargets[INDEX(RenderTarget::ScreenSpaceAmbientOcclusion)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::Lighting)], 3),
			renderTargets[INDEX(RenderTarget::DirectionalShadow)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::Lighting)], 4)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the bloom descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::Bloom)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::Scene)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::Bloom)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the bloom horizontal blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::BloomHorizontalBlur)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::Bloom)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::BloomHorizontalBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the bloom vertical blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::BloomVerticalBlur)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::BloomIntermediate)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::BloomVerticalBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the bloom horizontal blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingBloom)], descriptorSetLayouts[INDEX(RenderDataTableLayout::PostProcessingBloom)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 2> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::Scene)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingBloom)], 0),
			renderTargets[INDEX(RenderTarget::Bloom)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingBloom)], 1)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the post processing fog horizontal blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFogHorizontalBlur)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFogHorizontalBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the post processing fog vertical blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFogVerticalBlur)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::FogBlurIntermediate)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFogVerticalBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the post processing fog descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFog)], descriptorSetLayouts[INDEX(RenderDataTableLayout::PostProcessingFog)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 4> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFog)], 0),
			renderTargets[INDEX(RenderTarget::Scene)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFog)], 1),
			renderTargets[INDEX(RenderTarget::FogBlur)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFog)], 2),
			renderTargets[INDEX(RenderTarget::DirectionalShadowMap)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingFog)], 3)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the horizontal blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingHorizontalBlur)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::Scene)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingHorizontalBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the vertical blur descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingVerticalBlur)], descriptorSetLayouts[INDEX(RenderDataTableLayout::GaussianBlur)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::BlurIntermediate)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessingVerticalBlur)], 0)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Initialize the post processing descriptor set.
		VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessing)], descriptorSetLayouts[INDEX(RenderDataTableLayout::PostProcessing)]);

		//Update the write descriptor sets.
		StaticArray<VkWriteDescriptorSet, 4> writeDescriptorSets
		{
			uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessing)], 1),
			renderTargets[INDEX(RenderTarget::Scene)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessing)], 2),
			renderTargets[INDEX(RenderTarget::Bloom)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessing)], 3),
			renderTargets[INDEX(RenderTarget::Blur)]->GetWriteDescriptorSet(descriptorSets[INDEX(RenderDataTable::PostProcessing)], 4)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
	}
}

/*
*	Execute asynchronous tasks.
*/
void VulkanRenderingSystem::ExecuteAsynchronousTasks() NOEXCEPT
{
	//Execute the asynchronous tasks.
	static Task particleSystemUpdateTask{ [](void *const RESTRICT arguments)
	{
		static_cast<VulkanRenderingSystem *const RESTRICT>(arguments)->UpdateParticleSystemProperties();
	}, this, &taskSemaphores[INDEX(TaskSemaphore::UpdateParticleSystemProperties)] };

	TaskSystem::Instance->ExecuteTask(&particleSystemUpdateTask);
}

/*
*	Updates the post processing data.
*/
void VulkanRenderingSystem::UpdatePostProcessingData() NOEXCEPT
{
	if (PostProcessingManager::Instance->GetHasBeenUpdated())
	{
		PostProcessingManager::Instance->SetHasBeenUpdated(false);

		PostProcessingUniformData data;

		data.bloomStrength = PostProcessingManager::Instance->GetBloomStrength();
		data.blurStrength = PostProcessingManager::Instance->GetBlurStrength();
		data.chromaticAberrationStrength = PostProcessingManager::Instance->GetChromaticAbberationStrength();
		data.saturationStrength = PostProcessingManager::Instance->GetSaturationStrength();
		data.sharpenStrength = PostProcessingManager::Instance->GetSharpenStrength();

		uniformBuffers[UniformBuffer::PostProcessingUniformDataBuffer]->UploadData(&data);
	}
}

/*
*	Begins the frame.
*/
void VulkanRenderingSystem::BeginFrame() NOEXCEPT
{
	//Set the current frame.
	frameData.SetCurrentFrame(VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());

	//Set the current dynamic uniform data descriptor set.
	currentDynamicUniformDataDescriptorSet = frameData.GetCurrentDynamicUniformDataDescriptorSet();

	//Set the current environment descriptor set.
	currentEnvironmentDataDescriptorSet = frameData.GetCurrentEnvironmentDescriptorSet();

	//Wait for the current fence to finish.
	frameData.GetCurrentFence()->WaitFor();

	//Reset the current fence.
	frameData.GetCurrentFence()->Reset();
}

/*
*	Concatenates all secondary command buffers into the previous one.
*/
void VulkanRenderingSystem::ConcatenateCommandBuffers() NOEXCEPT
{
	//Begin the current primary command buffer.
	VulkanCommandBuffer *const RESTRICT currentPrimaryCommandBuffer{ frameData.GetCurrentPrimaryCommandBuffer() };
	currentPrimaryCommandBuffer->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Iterate over all render passes an concatenate their command buffers into the primary command buffer.
	const StaticArray<RenderPass *RESTRICT, INDEX(RenderPassStage::NumberOfRenderPassStages)>& renderPasses{ RenderingSystem::Instance->GetRenderPasses() };

	for (const RenderPass *const RESTRICT renderPass : renderPasses)
	{
		//Calculate the number of clear values.
		uint32 numberOfClearValues{ 0 };

		if (renderPass->GetDepthAttachmentLoadOperator() == AttachmentLoadOperator::Clear)
		{
			++numberOfClearValues;
		}

		if (renderPass->GetColorAttachmentLoadOperator() == AttachmentLoadOperator::Clear)
		{
			numberOfClearValues += static_cast<uint32>(renderPass->GetRenderTargets().Size());
		}

		//Calculate which framebuffer should be used.
		const uint32 frameBuffer{ renderPass->GetRenderTargets().Size() > 0 && renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameIndex() : static_cast<uint32>(0) };

		//Wait for the render pass to finish it's render.
		renderPass->WaitForRender();

		//If a clear should be done, the render pass needs to be begun in order for the clear to happen.
		if (renderPass->IncludeInRender() || numberOfClearValues > 0)
		{
			if (numberOfClearValues > 0)
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear(pipelines[INDEX(renderPass->GetStage())]->GetRenderPass(), frameBuffer, VkExtent2D{ renderPass->GetRenderResolution().width, renderPass->GetRenderResolution().height }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, numberOfClearValues);
			}
			
			else
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPass(pipelines[INDEX(renderPass->GetStage())]->GetRenderPass(), frameBuffer, VkExtent2D{ renderPass->GetRenderResolution().width, renderPass->GetRenderResolution().height }, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
			}
		}

		if (renderPass->IncludeInRender())
		{
			//Record the execute commands.
			currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<const VulkanTranslationCommandBuffer *const RESTRICT>(renderPass->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());
		}

		if (renderPass->IncludeInRender() || numberOfClearValues > 0)
		{
			//End the render pass.
			currentPrimaryCommandBuffer->CommandEndRenderPass();
		}
	}
}

/*
*	Ends the frame.
*/
void VulkanRenderingSystem::EndFrame() NOEXCEPT
{
	//Wait for the particle system properties update to finish.
	taskSemaphores[INDEX(TaskSemaphore::UpdateParticleSystemProperties)].WaitFor();

	//End the current command buffer.
	frameData.GetCurrentPrimaryCommandBuffer()->End();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue()->Submit(*frameData.GetCurrentPrimaryCommandBuffer(), 1, semaphores[INDEX(GraphicsSemaphore::ImageAvailable)], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 1, semaphores[INDEX(GraphicsSemaphore::RenderFinished)], frameData.GetCurrentFence()->Get());
}

/*
*	Updates the properties of particle systems.
*/
void VulkanRenderingSystem::UpdateParticleSystemProperties() const NOEXCEPT
{
	//Go through every particle system component and re-upload the properties, since they might change.
	const uint64 numberOfParticleSystemComponents{ ComponentManager::GetNumberOfParticleSystemComponents() };

	//If there are none, just return.
	if (numberOfParticleSystemComponents == 0)
	{
		return;
	}

	const ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemComponents() };

	for (uint64 i = 0; i < numberOfParticleSystemComponents; ++i, ++component)
	{
		const VulkanParticleSystemProperties vulkanParticleSystemProperties{ component->properties };
		static_cast<VulkanUniformBuffer *const RESTRICT>(component->propertiesUniformBuffer)->UploadData(&vulkanParticleSystemProperties);
	}
}

/*
*	Updates the descriptor sets.
*/
void VulkanRenderingSystem::UpdateDescriptorSets() NOEXCEPT
{
	{
		//Update the environment descriptor set.
		VulkanDescriptorSet &environmentDescriptorSet{ *currentEnvironmentDataDescriptorSet };

		StaticArray<VkWriteDescriptorSet, 4> environmentWriteDescriptorSets;

		if (EnvironmentManager::Instance->GetNightEnvironmentMaterial())
		{
			environmentWriteDescriptorSets[0] = static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetNightEnvironmentMaterial().diffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 0);
			environmentWriteDescriptorSets[1] = static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetNightEnvironmentMaterial().diffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 1);
		}

		else
		{
			const EnvironmentMaterial defaultMaterial{ RenderingSystem::Instance->GetCommonEnvironmentMaterial(RenderingSystem::CommonEnvironmentMaterial::Teal) };

			environmentWriteDescriptorSets[0] = static_cast<const VulkanCubeMapTexture *const RESTRICT>(defaultMaterial.diffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 0);
			environmentWriteDescriptorSets[1] = static_cast<const VulkanCubeMapTexture *const RESTRICT>(defaultMaterial.diffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 1);
		}

		if (EnvironmentManager::Instance->GetDayEnvironmentMaterial())
		{
			environmentWriteDescriptorSets[2] = static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetDayEnvironmentMaterial().diffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 2);
			environmentWriteDescriptorSets[3] = static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetDayEnvironmentMaterial().diffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 3);
		}

		else
		{
			const EnvironmentMaterial defaultMaterial{ RenderingSystem::Instance->GetCommonEnvironmentMaterial(RenderingSystem::CommonEnvironmentMaterial::Teal) };

			environmentWriteDescriptorSets[2] = static_cast<const VulkanCubeMapTexture *const RESTRICT>(defaultMaterial.diffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 2);
			environmentWriteDescriptorSets[3] = static_cast<const VulkanCubeMapTexture *const RESTRICT>(defaultMaterial.diffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 3);
		}

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(environmentWriteDescriptorSets.Size()), environmentWriteDescriptorSets.Data(), 0, nullptr);
	}

	if (EnvironmentManager::Instance->GetOceanMaterial())
	{
		//Update the ocean descriptor set.
		VulkanDescriptorSet &oceanDescriptorSet{ *frameData.GetCurrentOceanDescriptorSet() };

		StaticArray<VkWriteDescriptorSet, 3> oceanWriteDescriptorSets
		{
			renderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetWriteDescriptorSet(oceanDescriptorSet, 0),
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetWriteDescriptorSet(oceanDescriptorSet, 1),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetOceanMaterial().normalMapTexture)->GetWriteDescriptorSet(oceanDescriptorSet, 2)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(oceanWriteDescriptorSets.Size()), oceanWriteDescriptorSets.Data(), 0, nullptr);
	}
}

/*
*	Updates the dynamic uniform data.
*/
void VulkanRenderingSystem::UpdateDynamicUniformData() NOEXCEPT
{
	//Calculate the camera data.
	const CameraEntity *const RESTRICT activeCamera{ RenderingSystem::Instance->GetActiveCamera() };
	Vector3 cameraWorldPosition = activeCamera->GetPosition();
	Vector3 forwardVector = activeCamera->GetForwardVector();
	Vector3 upVector = activeCamera->GetUpVector();

	const Matrix4 *const RESTRICT projectionMatrix{ RenderingSystem::Instance->GetProjectionMatrix() };
	const Matrix4 *const RESTRICT cameraMatrix{ RenderingSystem::Instance->GetCameraMatrix() };
	const Matrix4 *const RESTRICT viewMatrix{ RenderingSystem::Instance->GetViewMatrix() };

	Matrix4 inverseCameraMatrix{ *cameraMatrix };
	inverseCameraMatrix.Inverse();

	Matrix4 cameraOriginMatrix{ *cameraMatrix };
	cameraOriginMatrix.SetTranslation(Vector3(0.0f, 0.0f, 0.0f));

	Matrix4 inverseProjectionMatrix{ *projectionMatrix };
	inverseProjectionMatrix.Inverse();

	dynamicUniformData.cameraFieldOfViewCosine = CatalystMath::CosineDegrees(activeCamera->GetFieldOfView()) - 0.2f;
	dynamicUniformData.inverseCameraMatrix = inverseCameraMatrix;
	dynamicUniformData.inverseProjectionMatrix = inverseProjectionMatrix;
	dynamicUniformData.originViewMatrix = *projectionMatrix * cameraOriginMatrix;
	dynamicUniformData.viewMatrix = *viewMatrix;
	dynamicUniformData.cameraForwardVector = forwardVector;
	dynamicUniformData.cameraWorldPosition = cameraWorldPosition;

	const uint64 numberOfDirectionalLightEntityComponents{ ComponentManager::GetNumberOfDirectionalLightComponents() };

	if (numberOfDirectionalLightEntityComponents > 0)
	{
		const DirectionalLightComponent *RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightComponents() };

		dynamicUniformData.directionalLightIntensity = directionalLightComponent->intensity;
		dynamicUniformData.directionalLightViewMatrix = RenderingUtilities::CalculateDirectionalLightViewMatrix();
		dynamicUniformData.directionalLightDirection = Vector3(0.0f, 0.0f, -1.0f).Rotated(directionalLightComponent->rotation);
		dynamicUniformData.directionalLightColor = directionalLightComponent->color;
		dynamicUniformData.directionalLightScreenSpacePosition = *viewMatrix * Vector4(-dynamicUniformData.directionalLightDirection.X * 100.0f + cameraWorldPosition.X, -dynamicUniformData.directionalLightDirection.Y * 100.0f + cameraWorldPosition.Y, -dynamicUniformData.directionalLightDirection.Z * 100.0f + cameraWorldPosition.Z, 1.0f);
		dynamicUniformData.directionalLightScreenSpacePosition.X /= dynamicUniformData.directionalLightScreenSpacePosition.W;
		dynamicUniformData.directionalLightScreenSpacePosition.Y /= dynamicUniformData.directionalLightScreenSpacePosition.W;
		dynamicUniformData.directionalLightScreenSpacePosition.Z /= dynamicUniformData.directionalLightScreenSpacePosition.W;
	}

	else
	{
		dynamicUniformData.directionalLightIntensity = 0.0f;
		dynamicUniformData.directionalLightViewMatrix = Matrix4();
		dynamicUniformData.directionalLightDirection = Vector3(0.0f, 0.0f, 0.0f);
		dynamicUniformData.directionalLightColor = Vector3(0.0f, 0.0f, 0.0f);
		dynamicUniformData.directionalLightScreenSpacePosition = Vector3(0.0f, 0.0f, 0.0f);
	}

	dynamicUniformData.environmentBlend = EnvironmentManager::Instance->GetEnvironmentBlend();

	dynamicUniformData.deltaTime = EngineSystem::Instance->GetDeltaTime();
	dynamicUniformData.totalGameTime = EngineSystem::Instance->GetTotalGameTime();

	uint64 counter = 0;

	const uint64 numberOfPointLightEntityComponents{ ComponentManager::GetNumberOfPointLightComponents() };
	const PointLightComponent *RESTRICT pointLightComponent{ ComponentManager::GetPointLightComponents() };

	dynamicUniformData.numberOfPointLights = static_cast<int32>(numberOfPointLightEntityComponents);

	for (uint64 i = 0; i < numberOfPointLightEntityComponents; ++i, ++pointLightComponent)
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

	const uint64 numberOfSpotLightEntityComponents{ ComponentManager::GetNumberOfSpotLightComponents() };
	const SpotLightComponent *RESTRICT spotLightComponent{ ComponentManager::GetSpotLightComponents() };

	dynamicUniformData.numberOfSpotLights = static_cast<int32>(numberOfSpotLightEntityComponents);

	for (uint64 i = 0; i < numberOfSpotLightEntityComponents; ++i, ++spotLightComponent)
	{
		if (!spotLightComponent->enabled)
		{
			--dynamicUniformData.numberOfSpotLights;

			continue;
		}

		dynamicUniformData.spotLightAttenuationDistances[counter] = spotLightComponent->attenuationDistance;
		dynamicUniformData.spotLightIntensities[counter] = spotLightComponent->intensity;
		dynamicUniformData.spotLightInnerCutoffAngles[counter] = CatalystMath::CosineDegrees(spotLightComponent->innerCutoffAngle);
		dynamicUniformData.spotLightOuterCutoffAngles[counter] = CatalystMath::CosineDegrees(spotLightComponent->outerCutoffAngle);
		dynamicUniformData.spotLightColors[counter] = spotLightComponent->color;
		dynamicUniformData.spotLightDirections[counter] = Vector3(0.0f, -1.0f, 0.0f).Rotated(spotLightComponent->rotation);
		dynamicUniformData.spotLightDirections[counter].Y *= -1.0f;
		dynamicUniformData.spotLightWorldPositions[counter] = spotLightComponent->position;

		++counter;
	}

	//Update the physics data.
	dynamicUniformData.windStrength = PhysicsSystem::Instance->GetWindStrength();
	dynamicUniformData.windDirection = PhysicsSystem::Instance->GetWindDirection();

	//Upload the dynamic uniform data to the uniform buffer.
	frameData.GetCurrentDynamicUniformDataBuffer()->UploadData(static_cast<void*>(&dynamicUniformData));
}