//Header file.
#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>

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
#include <Entities/InitializationData/TerrainInitializationData.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Matrix3.h>

//Multithreading.
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/PostProcessingUniformData.h>
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/TerrainMaterial.h>
#include <Rendering/Engine/TextureData.h>
#include <Rendering/Engine/VegetationMaterial.h>
#include <Rendering/Engine/RenderPasses/RenderPasses.h>
#include <Rendering/ShaderData/Vulkan/VulkanShaderData.h>
#include <Rendering/Translation/Vulkan/VulkanParticleSystemProperties.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationCommandBuffer.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanRenderingSystem);

/*
*	Initializes the Vulkan rendering system.
*/
void VulkanRenderingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize();

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

	//Initialize all Vulkan render passes.
	InitializeVulkanRenderPasses();

	//Initialize the Vulkan frame data.
	frameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages(), descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::DynamicUniformData)], descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Environment)], descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Ocean)]);
}

/*
*	Pre-updates the Vulkan rendering system synchronously.
*/
void VulkanRenderingSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
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
}

/*
*	Releases the Vulkan rendering system.
*/
void VulkanRenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release all descriptor set layouts.
	for (uint32 i = 0; i < INDEX(CommonRenderDataTableLayout::NumberOfCommonRenderDataTableLayouts); ++i)
	{
		descriptorSetLayouts[i].Release();
	}

	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Returns the current frame index.
*/
uint8 VulkanRenderingSystem::GetCurrentFrameIndex() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();
}

/*
*	Returns the given render target.
*/
RenderTargetHandle VulkanRenderingSystem::GetRenderTarget(const RenderTarget renderTarget) NOEXCEPT
{
	return renderTargets[INDEX(renderTarget)];
}

/*
*	Returns the given uniform buffer.
*/
UniformBufferHandle VulkanRenderingSystem::GetUniformBuffer(const UniformBuffer uniformBuffer) NOEXCEPT
{
	return uniformBuffers[INDEX(uniformBuffer)];
}

/*
*	Creates a constant buffer.
*/
ConstantBufferHandle VulkanRenderingSystem::CreateConstantBuffer(const void *RESTRICT data[], const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT
{
	return reinterpret_cast<ConstantBufferHandle>(VulkanInterface::Instance->CreateConstantBuffer(data, reinterpret_cast<const VkDeviceSize *const RESTRICT>(dataSizes), dataChunks)->Get());
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
	environmentMaterial.diffuseTexture = static_cast<TextureCubeMapHandle>(VulkanInterface::Instance->CreateCubeMapTexture(environmentMaterialData._DiffuseData.Data(), environmentMaterialData._DiffuseResolution, environmentMaterialData._DiffuseResolution));

	//Create the diffuse irradiance texture.
	environmentMaterial.diffuseIrradianceTexture = static_cast<TextureCubeMapHandle>(VulkanInterface::Instance->CreateCubeMapTexture(environmentMaterialData._DiffuseIrradianceData.Data(), environmentMaterialData._DiffuseIrradianceResolution, environmentMaterialData._DiffuseIrradianceResolution));
}

/*
*	Initializes a terrain entity.
*/
void VulkanRenderingSystem::InitializeTerrainEntity(const TerrainEntity *const RESTRICT entity, const TerrainInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Fill the terrain entity components with the relevant data.
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetTerrainFrustumCullingComponents()[entity->GetComponentsIndex()] };
	TerrainComponent &terrainComponent{ ComponentManager::GetTerrainTerrainComponents()[entity->GetComponentsIndex()] };
	TerrainRenderComponent &renderComponent{ ComponentManager::GetTerrainTerrainRenderComponents()[entity->GetComponentsIndex()] };

	frustumCullingComponent._AxisAlignedBoundingBox = data->_AxisAlignedBoundingBox;

	Texture2DHandle terrainPropertiesTexture = Create2DTexture(TextureData(TextureDataContainer(data->_TerrainProperties), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Nearest, TextureFormat::R32G32B32A32_Float));

	terrainComponent._TerrainUniformData = data->_TerrainUniformData;
	terrainComponent._UniformBuffer = VulkanInterface::Instance->CreateUniformBuffer(sizeof(TerrainUniformData));
	static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent._UniformBuffer)->UploadData(&terrainComponent._TerrainUniformData);
 	terrainComponent._TerrainProperties = data->_TerrainProperties;
	terrainComponent._TerrainPropertiesTexture = terrainPropertiesTexture;

	//Create the descriptor set.
	renderComponent._RenderDataTable = VulkanInterface::Instance->CreateDescriptorSet(descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Terrain)]);
	VulkanDescriptorSet& newDescriptorSet{ *static_cast<VulkanDescriptorSet *const RESTRICT>(renderComponent._RenderDataTable) };

	StaticArray<VkWriteDescriptorSet, 18> writeDescriptorSets
	{
		static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent._UniformBuffer)->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(terrainPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_LayerWeightsTexture)->GetWriteDescriptorSet(newDescriptorSet, 3),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.firstLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 4),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.firstLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 5),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.firstLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 6),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.secondLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 7),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.secondLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 8),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.secondLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 9),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.thirdLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 10),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.thirdLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 11),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.thirdLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 12),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.fourthLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 13),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.fourthLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 14),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.fourthLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 15),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.fifthLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 16),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.fifthLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 17),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial.fifthLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 18)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
}

/*
*	Initializes a static physical entity.
*/
void VulkanRenderingSystem::InitializeStaticPhysicalEntity(StaticPhysicalEntity &staticPhysicalEntity, const PhysicalModel &model, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) const NOEXCEPT
{
	//Cache relevant data.
	const PhysicalMaterial &material = model._Material;
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetStaticPhysicalFrustumCullingComponents()[staticPhysicalEntity.GetComponentsIndex()] };
	StaticPhysicalRenderComponent &renderComponent{ ComponentManager::GetStaticPhysicalStaticPhysicalRenderComponents()[staticPhysicalEntity.GetComponentsIndex()] };
	TransformComponent &transformComponent{ ComponentManager::GetStaticPhysicalTransformComponents()[staticPhysicalEntity.GetComponentsIndex()] };

	//Allocate the descriptor set.
	renderComponent._RenderDataTable = VulkanInterface::Instance->CreateDescriptorSet(descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Physical)]);
	VulkanDescriptorSet& newDescriptorSet{ *static_cast<VulkanDescriptorSet *const RESTRICT>(renderComponent._RenderDataTable) };

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 3> writeDescriptorSets
	{
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.normalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.materialPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 3)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);

	//Fill the static physical entity components with the relevant data.
	frustumCullingComponent._AxisAlignedBoundingBox = model._AxisAlignedBoundingBox;
	renderComponent._ModelMatrix = Matrix4(position, rotation, scale);
	renderComponent._Buffer = model._Buffer;
	renderComponent._IndexOffset = model._IndexOffset;
	renderComponent._IndexCount = model._IndexCount;
	transformComponent._Position = position;
	transformComponent._Rotation = rotation;
	transformComponent._Scale = scale;
}

/*
*	Initializes an instanced physical entity.
*/
void VulkanRenderingSystem::InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Cache relevant data.
	const PhysicalMaterial &material = model._Material;
	InstancedPhysicalRenderComponent &renderComponent{ ComponentManager::GetInstancedPhysicalInstancedPhysicalRenderComponents()[entity.GetComponentsIndex()] };

	//Allocate the descriptor set.
	renderComponent._RenderDataTable = VulkanInterface::Instance->CreateDescriptorSet(descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Physical)]);
	VulkanDescriptorSet& newDescriptorSet{ *static_cast<VulkanDescriptorSet *const RESTRICT>(renderComponent._RenderDataTable) };

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
	renderComponent._ModelBuffer = model._Buffer;
	renderComponent._TransformationsBuffer = reinterpret_cast<ConstantBufferHandle>(transformationsBuffer->Get());
	renderComponent._IndexOffset = model._IndexOffset;
	renderComponent._IndexCount = model._IndexCount;
	renderComponent._InstanceCount = static_cast<uint32>(transformations.Size());
}

/*
*	Initializes a vegetation entity.
*/
void VulkanRenderingSystem::InitializeVegetationEntity(const VegetationEntity &entity, const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) const NOEXCEPT
{
	//Get the components.
	VegetationComponent &renderComponent{ ComponentManager::GetVegetationVegetationComponents()[entity.GetComponentsIndex()] };
	VegetationCullingComponent &cullingComponent{ ComponentManager::GetVegetationVegetationCullingComponents()[entity.GetComponentsIndex()] };

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
	renderComponent._RenderDataTable = VulkanInterface::Instance->CreateDescriptorSet(descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Vegetation)]);
	VulkanDescriptorSet& newDescriptorSet{ *static_cast<VulkanDescriptorSet *const RESTRICT>(renderComponent._RenderDataTable) };

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
	renderComponent._TransformationsBuffer = reinterpret_cast<ConstantBufferHandle>(transformationsBuffer->Get());
}

/*
*	Initializes a particle system entity.
*/
void VulkanRenderingSystem::InitializeParticleSystemEntity(const ParticleSystemEntity &entity, const ParticleMaterial &material, const ParticleSystemProperties &properties) const NOEXCEPT
{
	//Cache relevant data.
	ParticleSystemComponent &component{ ComponentManager::GetParticleSystemParticleSystemComponents()[entity.GetComponentsIndex()] };
	ParticleSystemRenderComponent &renderComponent{ ComponentManager::GetParticleSystemParticleSystemRenderComponents()[entity.GetComponentsIndex()] };

	//Create the uniform buffer.
	VulkanUniformBuffer *const RESTRICT uniformBuffer{ VulkanInterface::Instance->CreateUniformBuffer(static_cast<VkDeviceSize>(sizeof(VulkanParticleSystemProperties))) };
	const VulkanParticleSystemProperties vulkanParticleSystemProperties{ properties };
	uniformBuffer->UploadData(&vulkanParticleSystemProperties);

	//Create the descriptor set.
	renderComponent._RenderDataTable = VulkanInterface::Instance->CreateDescriptorSet(descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::ParticleSystem)]);
	VulkanDescriptorSet& newDescriptorSet{ *static_cast<VulkanDescriptorSet *const RESTRICT>(renderComponent._RenderDataTable) };

	StaticArray<VkWriteDescriptorSet, 2> particleSystemWriteDescriptorSets
	{
		uniformBuffer->GetWriteDescriptorSet(newDescriptorSet, 0),
		static_cast<const Vulkan2DTexture *RESTRICT>(material.albedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 1)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(particleSystemWriteDescriptorSets.Size()), particleSystemWriteDescriptorSets.Data(), 0, nullptr);

	//Set up the particle system's components.
	component._Properties = properties;
	component._PropertiesUniformBuffer = uniformBuffer;
	renderComponent._ParticleSystemRandomSeed = CatalystMath::RandomFloatInRange(0.0f, 1.0f);
	renderComponent._ParticleSystemStartingTime = EngineSystem::Instance->GetTotalGameTime();
	renderComponent._InstanceCount = CatalystMath::Round<uint32>(properties.lifetime / properties.spawnFrequency);
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

	parameters.blendEnable = renderPass->IsBlendEnabled();
	parameters.colorAttachmentCount = static_cast<uint32>(renderPass->GetRenderTargets().Size());
	parameters.cullMode = VulkanTranslationUtilities::GetVulkanCullMode(renderPass->GetCullMode());
	parameters.depthCompareOp = VulkanTranslationUtilities::GetVulkanCompareOperator(renderPass->GetDepthCompareOperator());
	parameters.depthTestEnable = renderPass->IsDepthTestEnabled();
	parameters.depthWriteEnable = renderPass->IsDepthWriteEnabled();

	DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
	pipelineDescriptorSetLayouts.Reserve(renderPass->GetRenderDataTableLayouts().Size());

	for (RenderDataTableLayoutHandle renderDataTableLayout : renderPass->GetRenderDataTableLayouts())
	{
		pipelineDescriptorSetLayouts.EmplaceFast(*static_cast<VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
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
	parameters.subpass = renderPass->GetSubStageIndex();
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
	parameters.viewportExtent = renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution().width, renderPass->GetRenderResolution().height };

	parameters.renderPass = vulkanRenderPassMainStageData[INDEX(renderPass->GetMainStage())].renderPass;

	//Create the pipeline!
	pipelines[INDEX(renderPass->GetSubStage())] = VulkanInterface::Instance->CreatePipeline(parameters);

	//Update the Vulkan render pass data.
	vulkanRenderPassData[INDEX(renderPass->GetSubStage())].framebuffers.Reserve(vulkanRenderPassMainStageData[INDEX(renderPass->GetMainStage())].frameBuffers.Size());

	for (VulkanFramebuffer *RESTRICT vulkanFrameBuffer : vulkanRenderPassMainStageData[INDEX(renderPass->GetMainStage())].frameBuffers)
	{
		vulkanRenderPassData[INDEX(renderPass->GetSubStage())].framebuffers.EmplaceFast(vulkanFrameBuffer->Get());
	}

	vulkanRenderPassData[INDEX(renderPass->GetSubStage())].pipeline = pipelines[INDEX(renderPass->GetSubStage())]->Get();
	vulkanRenderPassData[INDEX(renderPass->GetSubStage())].pipelineLayout = pipelines[INDEX(renderPass->GetSubStage())]->GetPipelineLayout();
	vulkanRenderPassData[INDEX(renderPass->GetSubStage())].renderPass = vulkanRenderPassMainStageData[INDEX(renderPass->GetMainStage())].renderPass->Get();

	renderPass->SetData(&vulkanRenderPassData[INDEX(renderPass->GetSubStage())]);

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
*	Creates a render data table layout.
*/
void VulkanRenderingSystem::CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 numberOfBindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	//Translate from the API-agnostic structure to the Vulkan-specific.
	DynamicArray<VkDescriptorSetLayoutBinding> vulkanBindings;
	vulkanBindings.Reserve(numberOfBindings);

	for (uint32 i = 0; i < numberOfBindings; ++i)
	{
		const RenderDataTableLayoutBinding &binding{ bindings[i] };

		vulkanBindings.EmplaceFast(VulkanUtilities::CreateDescriptorSetLayoutBinding(binding.binding, VulkanTranslationUtilities::GetVulkanDescriptorType(binding.type), VulkanTranslationUtilities::GetVulkanShaderStage(binding.shaderStage)));
	}

	*handle = VulkanInterface::Instance->CreateDescriptorSetLayout(vulkanBindings.Data(), numberOfBindings);
}

/*
*	Creates a render data table.
*/
void VulkanRenderingSystem::CreateRenderDataTable(const RenderDataTableLayoutHandle renderDataTableLayout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	*handle = VulkanInterface::Instance->CreateDescriptorSet(*static_cast<const VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
}

/*
*	Updates a render data table.
*/
void VulkanRenderingSystem::UpdateRenderDataTable(const RenderDataTableUpdateInformation information, RenderDataTableHandle handle) const NOEXCEPT
{
	VulkanDescriptorSet *const RESTRICT descriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(handle) };
	VkWriteDescriptorSet writeDescriptorSet;

	switch (information.type)
	{
		case RenderDataTableUpdateInformation::Type::RenderTarget:
		{
			writeDescriptorSet = static_cast<VulkanRenderTarget *const RESTRICT>(information.handle)->GetWriteDescriptorSet(*descriptorSet, information.binding);

			break;
		}

		case RenderDataTableUpdateInformation::Type::Texture2D:
		{
			writeDescriptorSet = static_cast<Vulkan2DTexture *const RESTRICT>(information.handle)->GetWriteDescriptorSet(*descriptorSet, information.binding);

			break;
		}

		case RenderDataTableUpdateInformation::Type::UniformBuffer:
		{
			writeDescriptorSet = static_cast<VulkanUniformBuffer *const RESTRICT>(information.handle)->GetWriteDescriptorSet(*descriptorSet, information.binding);

			break;
		}

#if !defined(CATALYST_FINAL)
		default:
		{
			BREAKPOINT();
		}
#endif
	}

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Returns the current dynamic uniform data descriptor set.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCurrentDynamicUniformDataRenderDataTable() NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(frameData.GetCurrentDynamicUniformDataRenderDataTable());
}

/*
*	Returns the current environment data descriptor set.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCurrentEnvironmentRenderDataTable() NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(frameData.GetCurrentEnvironmentDescriptorSet());
}

/*
*	Returns the current ocean descriptor set.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCurrentOceanRenderDataTable() NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(frameData.GetCurrentOceanRenderDataTable());
}

/*
*	Returns the given common render data table layout.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(&descriptorSetLayouts[INDEX(commonRenderDataTableLayout)]);
}

/*
*	Initializes all render targets.
*/
void VulkanRenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Get the scaled extent.
	const VkExtent2D scaledExtent{ VulkanTranslationUtilities::GetVulkanExtent(RenderingSystem::Instance->GetScaledResolution()) };

	//Initialize all depth buffers.
	depthBuffers[INDEX(DepthBuffer::DirectionalLight)] = VulkanInterface::Instance->CreateDepthBuffer({ EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution });
	depthBuffers[INDEX(DepthBuffer::SceneBuffer)] = VulkanInterface::Instance->CreateDepthBuffer(scaledExtent);

	//Initialize all render targets.
	renderTargets[INDEX(RenderTarget::DirectionalShadowMap)] = VulkanInterface::Instance->CreateRenderTarget({ EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution }, VK_FORMAT_R8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::DirectionalShadow)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R8_UNORM, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	renderTargets[INDEX(RenderTarget::SceneBufferAlbedo)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R8G8B8A8_SNORM, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R8G8B8A8_SNORM, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::SceneIntermediate)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	renderTargets[INDEX(RenderTarget::Scene)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
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
			sample._X = CatalystMath::RandomFloatInRange(-1.0f, 1.0f);
			sample._Y = CatalystMath::RandomFloatInRange(-1.0f, 1.0f);
			sample._Z = CatalystMath::RandomFloatInRange(0.0f, 1.0f);
			sample._W = 0.0f;

			sample.Normalize();

			float scale{ CatalystMath::RandomFloatInRange(0.0f, 1.0f) };
			scale = CatalystMath::LinearlyInterpolate(0.1f, 1.0f, scale * scale);

			sample *= scale;
		}

		uniformBuffers[INDEX(UniformBuffer::ScreenSpaceAmbientOcclusionSamples)] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(Vector4) * RenderingConstants::SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLE_KERNEL_SIZE);
		uniformBuffers[INDEX(UniformBuffer::ScreenSpaceAmbientOcclusionSamples)]->UploadData(samples.Data());
	}
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

		descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::DynamicUniformData)].Initialize(1, dynamicUniformDataDescriptorSetLayoutBindings.Data());
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

		descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Environment)].Initialize(static_cast<uint32>(environmentDescriptorSetLayoutBindings.Size()), environmentDescriptorSetLayoutBindings.Data());
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

		descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Terrain)].Initialize(18, terrainDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the static physical descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> staticPhysicalDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Physical)].Initialize(3, staticPhysicalDescriptorSetLayoutBindings.Data());
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

		descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Vegetation)].Initialize(static_cast<uint32>(vegetationDescriptorSetLayoutBindings.Size()), vegetationDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the ocean descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 3> oceanDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Ocean)].Initialize(static_cast<uint32>(oceanDescriptorSetLayoutBindings.Size()), oceanDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the particle system descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 2> particleSystemDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::ParticleSystem)].Initialize(static_cast<uint32>(particleSystemDescriptorSetLayoutBindings.Size()), particleSystemDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the bloom descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> descriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		};

		descriptorSetLayouts[INDEX(CommonRenderDataTableLayout::GaussianBlur)].Initialize(static_cast<uint32>(descriptorSetLayoutBindings.Size()), descriptorSetLayoutBindings.Data());
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
		//Initialize the box blur shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBoxBlurFragmentShaderData(data);
		shaderModules[INDEX(Shader::BoxBlurFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
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
		//Initialize the outline fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetOutlineFragmentShaderData(data);
		shaderModules[INDEX(Shader::OutlineFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
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
*	Initializes all Vulkan render passes.
*/
void VulkanRenderingSystem::InitializeVulkanRenderPasses() NOEXCEPT
{
	//Initialize the directional shadow render pass.
	{
		constexpr uint64 NUMBER_OF_DIRECTIONAL_SHADOW_SUBPASSES{ 3 };

		constexpr uint32 DEPTH_BUFFER_INDEX{ 0 };
		constexpr uint32 SHADOW_MAP_INDEX{ 1 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 2> attachmenDescriptions
		{
			//Depth buffer.
			VulkanUtilities::CreateAttachmentDescription(	depthBuffers[INDEX(DepthBuffer::DirectionalLight)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),

			//Shadow map.
			VulkanUtilities::CreateAttachmentDescription(	renderTargets[INDEX(RenderTarget::DirectionalShadowMap)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		};

		renderPassParameters.attachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters.attachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SHADOW_MAP_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
		};

		constexpr VkAttachmentReference depthAttachmentReference{ DEPTH_BUFFER_INDEX, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		StaticArray<VkSubpassDescription, NUMBER_OF_DIRECTIONAL_SHADOW_SUBPASSES> subpassDescriptions;

		for (VkSubpassDescription &subpassDescription : subpassDescriptions)
		{
			subpassDescription = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			1,
																			colorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);
		}

		renderPassParameters.subpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters.subpassDescriptions = subpassDescriptions.Data();

		StaticArray<VkSubpassDependency, 2> subpassDependencies
		{
			VulkanUtilities::CreateSubpassDependency(	0,
														1,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	1,
														2,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT)
		};

		renderPassParameters.subpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters.subpassDependencies = subpassDependencies.Data();

		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)].renderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters.renderPass = vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)].renderPass->Get();
		
		StaticArray<VkImageView, 2> attachments
		{
			depthBuffers[INDEX(DepthBuffer::DirectionalLight)]->GetImageView(),
			renderTargets[INDEX(RenderTarget::DirectionalShadowMap)]->GetImageView()
		};

		framebufferParameters.attachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters.attachments = attachments.Data();
		framebufferParameters.extent = { EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution };

		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)].frameBuffers.Reserve(1);
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)].frameBuffers.EmplaceFast( VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)].numberOfAttachments = 2;
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)].shouldClear = true;
	}

	//Initialize the scene buffer render pass.
	{
		constexpr uint64 NUMBER_OF_SCENE_BUFFER_SUBPASSES{ 10 };

		constexpr uint32 DEPTH_BUFFER_INDEX{ 0 };
		constexpr uint32 ALBEDO_INDEX{ 1 };
		constexpr uint32 NORMAL_DEPTH_INDEX{ 2 };
		constexpr uint32 MATERIAL_PROPERTIES_INDEX{ 3 };
		constexpr uint32 DIRECTIONAL_SHADOW_INDEX{ 4 };
		constexpr uint32 SCENE_INDEX{ 5 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 6> attachmenDescriptions
		{
			//Depth buffer.
			VulkanUtilities::CreateAttachmentDescription(	depthBuffers[INDEX(DepthBuffer::SceneBuffer)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),
			
			//Albedo.
			VulkanUtilities::CreateAttachmentDescription(	renderTargets[INDEX(RenderTarget::SceneBufferAlbedo)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Normal depth.
			VulkanUtilities::CreateAttachmentDescription(	renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Material properties.
			VulkanUtilities::CreateAttachmentDescription(	renderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Directional shadow.
			VulkanUtilities::CreateAttachmentDescription(	renderTargets[INDEX(RenderTarget::DirectionalShadow)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	renderTargets[INDEX(RenderTarget::Scene)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		};

		renderPassParameters.attachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters.attachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 3> sceneBufferColorAttachmentReferences
		{
			VkAttachmentReference{ ALBEDO_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			VkAttachmentReference{ NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			VkAttachmentReference{ MATERIAL_PROPERTIES_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
		};

		constexpr VkAttachmentReference depthAttachmentReference{ DEPTH_BUFFER_INDEX, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		StaticArray<VkSubpassDescription, NUMBER_OF_SCENE_BUFFER_SUBPASSES> subpassDescriptions;

		subpassDescriptions[0] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[1] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[2] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[3] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[4] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		constexpr VkAttachmentReference normalDepthInputAttachmentReference{ VulkanUtilities::CreateAttachmentReference(NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) };

		constexpr VkAttachmentReference directionalShadowColorAttachmentReference{ VulkanUtilities::CreateAttachmentReference(DIRECTIONAL_SHADOW_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) };

		subpassDescriptions[5] = VulkanUtilities::CreateSubpassDescription(	1,
																			&normalDepthInputAttachmentReference,
																			1,
																			&directionalShadowColorAttachmentReference,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		constexpr StaticArray<const VkAttachmentReference, 3> lightingInputAttachmentReferences
		{
			VkAttachmentReference{ ALBEDO_INDEX, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
			VkAttachmentReference{ NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
			VkAttachmentReference{ MATERIAL_PROPERTIES_INDEX, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
		};

		constexpr VkAttachmentReference sceneColorAttachmentReference{ VulkanUtilities::CreateAttachmentReference(SCENE_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) };

		subpassDescriptions[6] = VulkanUtilities::CreateSubpassDescription(	static_cast<uint32>(lightingInputAttachmentReferences.Size()),
																			lightingInputAttachmentReferences.Data(),
																			1,
																			&sceneColorAttachmentReference,
																			nullptr,
																			0,
																			nullptr);

		subpassDescriptions[7] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			1,
																			&sceneColorAttachmentReference,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[8] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			1,
																			&sceneColorAttachmentReference,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		constexpr StaticArray<const VkAttachmentReference, 2> particleSystemAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL },
			VkAttachmentReference{ NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		subpassDescriptions[9] = VulkanUtilities::CreateSubpassDescription(	static_cast<uint32>(particleSystemAttachmentReferences.Size()),
																			particleSystemAttachmentReferences.Data(),
																			static_cast<uint32>(particleSystemAttachmentReferences.Size()),
																			particleSystemAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		renderPassParameters.subpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters.subpassDescriptions = subpassDescriptions.Data();

		StaticArray<VkSubpassDependency, NUMBER_OF_SCENE_BUFFER_SUBPASSES - 1> subpassDependencies
		{
			VulkanUtilities::CreateSubpassDependency(	0,
														1,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	1,
														2,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	2,
														3,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	3,
														4,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	4,
														5,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	5,
														6,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	6,
														7,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	7,
														8,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	8,
														9,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_DEPENDENCY_BY_REGION_BIT)
		};

		renderPassParameters.subpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters.subpassDependencies = subpassDependencies.Data();

		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)].renderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters.renderPass = vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)].renderPass->Get();

		StaticArray<VkImageView, 6> attachments
		{
			depthBuffers[INDEX(DepthBuffer::SceneBuffer)]->GetImageView(),
			renderTargets[INDEX(RenderTarget::SceneBufferAlbedo)]->GetImageView(),
			renderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetImageView(),
			renderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)]->GetImageView(),
			renderTargets[INDEX(RenderTarget::DirectionalShadow)]->GetImageView(),
			renderTargets[INDEX(RenderTarget::Scene)]->GetImageView()
		};

		framebufferParameters.attachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters.attachments = attachments.Data();
		framebufferParameters.extent = { RenderingSystem::Instance->GetScaledResolution().width, RenderingSystem::Instance->GetScaledResolution().height };

		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)].frameBuffers.Reserve(1);
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)].frameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)].numberOfAttachments = 6;
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)].shouldClear = true;
	}

	//Initialize the ocean render pass.
	{
		constexpr uint64 NUMBER_OF_OCEAN_SUBPASSES{ 1 };

		constexpr uint32 SCENE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Scene.
			VulkanUtilities::CreateAttachmentDescription(renderTargets[INDEX(RenderTarget::Scene)]->GetFormat(),
			VK_ATTACHMENT_LOAD_OP_LOAD,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		};

		renderPassParameters.attachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters.attachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
		};

		StaticArray<VkSubpassDescription, NUMBER_OF_OCEAN_SUBPASSES> subpassDescriptions
		{
			VulkanUtilities::CreateSubpassDescription(0,
			nullptr,
			1,
			colorAttachmentReferences.Data(),
			nullptr,
			0,
			nullptr)
		};

		renderPassParameters.subpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters.subpassDescriptions = subpassDescriptions.Data();

		renderPassParameters.subpassDependencyCount = 0;
		renderPassParameters.subpassDependencies = nullptr;

		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)].renderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters.renderPass = vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)].renderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			renderTargets[INDEX(RenderTarget::Scene)]->GetImageView()
		};

		framebufferParameters.attachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters.attachments = attachments.Data();
		framebufferParameters.extent = { RenderingSystem::Instance->GetScaledResolution().width, RenderingSystem::Instance->GetScaledResolution().height };

		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)].frameBuffers.Reserve(1);
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)].frameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)].numberOfAttachments = 1;
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)].shouldClear = false;
	}

	//Initialize the post processing final render pass.
	{
		constexpr uint64 NUMBER_OF_POST_PROCESSING_FINAL_SUBPASSES{ 1 };

		constexpr uint32 SCREEN_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Screen.
			VulkanUtilities::CreateAttachmentDescription(	VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		};

		renderPassParameters.attachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters.attachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCREEN_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
		};

		StaticArray<VkSubpassDescription, NUMBER_OF_POST_PROCESSING_FINAL_SUBPASSES> subpassDescriptions;

		for (VkSubpassDescription &subpassDescription : subpassDescriptions)
		{
			subpassDescription = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			1,
																			colorAttachmentReferences.Data(),
																			nullptr,
																			0,
																			nullptr);
		}

		renderPassParameters.subpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters.subpassDescriptions = subpassDescriptions.Data();

		renderPassParameters.subpassDependencyCount = 0;
		renderPassParameters.subpassDependencies = nullptr;

		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)].renderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffers.
		const DynamicArray<VkImageView> &swapchainImages{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)].frameBuffers.Reserve(swapchainImages.Size());

		for (VkImageView swapchainImage : swapchainImages)
		{
			VulkanFramebufferCreationParameters framebufferParameters;

			framebufferParameters.renderPass = vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)].renderPass->Get();

			framebufferParameters.attachmentCount = 1;
			framebufferParameters.attachments = &swapchainImage;
			framebufferParameters.extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

			vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)].frameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		}

		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)].numberOfAttachments = 1;
		vulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)].shouldClear = false;
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
	currentDynamicUniformDataDescriptorSet = frameData.GetCurrentDynamicUniformDataRenderDataTable();

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

	//Iterate over all render passes and concatenate their command buffers into the primary command buffer.
	RenderPassMainStage currentStage{ RenderPassMainStage::None };

	for (const RenderPass *const RESTRICT renderPass : RenderingSystem::Instance->GetRenderPasses())
	{
		//Wait for the render pass to finish it's render.
		renderPass->WaitForRender();

		//Begin a new render pass, if necessary.
		if (currentStage != renderPass->GetMainStage())
		{
			if (currentStage != RenderPassMainStage::None)
			{
				currentPrimaryCommandBuffer->CommandEndRenderPass();
			}

			currentStage = renderPass->GetMainStage();

			//Specialization - Copy scene to scene intermediate for the ocean render pass.
			if (currentStage == RenderPassMainStage::Ocean)
			{
				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
																		VK_ACCESS_TRANSFER_READ_BIT,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
																		renderTargets[INDEX(RenderTarget::Scene)]->GetImage(),
																		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	0,
																		VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
																		renderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetImage(),
																		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				const VkExtent2D sourceExtent{ renderTargets[INDEX(RenderTarget::Scene)]->GetExtent() };
				const VkExtent3D extent{ sourceExtent.width, sourceExtent.height, 0 };

				currentPrimaryCommandBuffer->CommandCopyImage(renderTargets[INDEX(RenderTarget::Scene)]->GetImage(), renderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetImage(), extent);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
																		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		renderTargets[INDEX(RenderTarget::Scene)]->GetImage(),
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_ACCESS_SHADER_READ_BIT,
																		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		renderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetImage(),
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);
			}

			if (vulkanRenderPassMainStageData[INDEX(currentStage)].shouldClear)
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear(	vulkanRenderPassMainStageData[INDEX(currentStage)].renderPass->Get(),
																				vulkanRenderPassMainStageData[INDEX(currentStage)].frameBuffers[renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameIndex() : 0]->Get(),
																				renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution().width, renderPass->GetRenderResolution().height },
																				VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, vulkanRenderPassMainStageData[INDEX(currentStage)].numberOfAttachments);
			}

			else
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPass(	vulkanRenderPassMainStageData[INDEX(currentStage)].renderPass->Get(),
																		vulkanRenderPassMainStageData[INDEX(currentStage)].frameBuffers[renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameIndex() : 0]->Get(),
																		renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution().width, renderPass->GetRenderResolution().height },
																		VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
			}
		}

		else
		{
			currentPrimaryCommandBuffer->CommandNextSubpass();
		}

		//Record the execute commands.
		if (renderPass->IncludeInRender())
		{
			currentPrimaryCommandBuffer->CommandExecuteCommands(static_cast<const VulkanTranslationCommandBuffer *const RESTRICT>(renderPass->GetCurrentCommandBuffer())->GetVulkanCommandBuffer().Get());
		}
	}

	//End the last remaining render pass.
	currentPrimaryCommandBuffer->CommandEndRenderPass();
}

/*
*	Ends the frame.
*/
void VulkanRenderingSystem::EndFrame() NOEXCEPT
{
	//End the current command buffer.
	frameData.GetCurrentPrimaryCommandBuffer()->End();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue()->Submit(*frameData.GetCurrentPrimaryCommandBuffer(), 1, semaphores[INDEX(GraphicsSemaphore::ImageAvailable)], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 1, semaphores[INDEX(GraphicsSemaphore::RenderFinished)], frameData.GetCurrentFence()->Get());
}

/*
*	Updates the descriptor sets.
*/
void VulkanRenderingSystem::UpdateDescriptorSets() NOEXCEPT
{
	{
		//Update the environment descriptor set.
		VulkanDescriptorSet &environmentDescriptorSet{ *currentEnvironmentDataDescriptorSet };

		StaticArray<VkWriteDescriptorSet, 4> environmentWriteDescriptorSets
		{
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetNightEnvironmentMaterial().diffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 0),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetNightEnvironmentMaterial().diffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 1),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetDayEnvironmentMaterial().diffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 2),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetDayEnvironmentMaterial().diffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 3)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(environmentWriteDescriptorSets.Size()), environmentWriteDescriptorSets.Data(), 0, nullptr);
	}

	{
		//Update the ocean descriptor set.
		VulkanDescriptorSet &oceanDescriptorSet{ *frameData.GetCurrentOceanRenderDataTable() };

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
		const DirectionalLightComponent *RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightDirectionalLightComponents() };

		dynamicUniformData.directionalLightIntensity = directionalLightComponent->_Intensity;
		dynamicUniformData.directionalLightViewMatrix = RenderingUtilities::CalculateDirectionalLightViewMatrix();
		dynamicUniformData.directionalLightDirection = Vector3(0.0f, 0.0f, -1.0f).Rotated(directionalLightComponent->_Rotation);
		dynamicUniformData.directionalLightColor = directionalLightComponent->_Color;
		dynamicUniformData.directionalLightScreenSpacePosition = *viewMatrix * Vector4(-dynamicUniformData.directionalLightDirection._X * 100.0f + cameraWorldPosition._X, -dynamicUniformData.directionalLightDirection._Y * 100.0f + cameraWorldPosition._Y, -dynamicUniformData.directionalLightDirection._Z * 100.0f + cameraWorldPosition._Z, 1.0f);
		dynamicUniformData.directionalLightScreenSpacePosition._X /= dynamicUniformData.directionalLightScreenSpacePosition._W;
		dynamicUniformData.directionalLightScreenSpacePosition._Y /= dynamicUniformData.directionalLightScreenSpacePosition._W;
		dynamicUniformData.directionalLightScreenSpacePosition._Z /= dynamicUniformData.directionalLightScreenSpacePosition._W;
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
	const PointLightComponent *RESTRICT pointLightComponent{ ComponentManager::GetPointLightPointLightComponents() };

	dynamicUniformData.numberOfPointLights = static_cast<int32>(numberOfPointLightEntityComponents);

	for (uint64 i = 0; i < numberOfPointLightEntityComponents; ++i, ++pointLightComponent)
	{
		if (!pointLightComponent->_Enabled)
		{
			--dynamicUniformData.numberOfPointLights;

			continue;
		}

		dynamicUniformData.pointLightAttenuationDistances[counter] = pointLightComponent->_AttenuationDistance;
		dynamicUniformData.pointLightIntensities[counter] = pointLightComponent->_Intensity;
		dynamicUniformData.pointLightColors[counter] = pointLightComponent->_Color;
		dynamicUniformData.pointLightWorldPositions[counter] = pointLightComponent->_Position;

		++counter;
	}

	counter = 0;

	const uint64 numberOfSpotLightEntityComponents{ ComponentManager::GetNumberOfSpotLightComponents() };
	const SpotLightComponent *RESTRICT spotLightComponent{ ComponentManager::GetSpotLightSpotLightComponents() };

	dynamicUniformData.numberOfSpotLights = static_cast<int32>(numberOfSpotLightEntityComponents);

	for (uint64 i = 0; i < numberOfSpotLightEntityComponents; ++i, ++spotLightComponent)
	{
		if (!spotLightComponent->_Enabled)
		{
			--dynamicUniformData.numberOfSpotLights;

			continue;
		}

		dynamicUniformData.spotLightAttenuationDistances[counter] = spotLightComponent->_AttenuationDistance;
		dynamicUniformData.spotLightIntensities[counter] = spotLightComponent->_Intensity;
		dynamicUniformData.spotLightInnerCutoffAngles[counter] = CatalystMath::CosineDegrees(spotLightComponent->_InnerCutoffAngle);
		dynamicUniformData.spotLightOuterCutoffAngles[counter] = CatalystMath::CosineDegrees(spotLightComponent->_OuterCutoffAngle);
		dynamicUniformData.spotLightColors[counter] = spotLightComponent->_Color;
		dynamicUniformData.spotLightDirections[counter] = Vector3(0.0f, -1.0f, 0.0f).Rotated(spotLightComponent->_Rotation);
		dynamicUniformData.spotLightDirections[counter]._Y *= -1.0f;
		dynamicUniformData.spotLightWorldPositions[counter] = spotLightComponent->_Position;

		++counter;
	}

	//Update the physics data.
	dynamicUniformData.windStrength = PhysicsSystem::Instance->GetWindStrength();
	dynamicUniformData.windDirection = PhysicsSystem::Instance->GetWindDirection();

	//Upload the dynamic uniform data to the uniform buffer.
	frameData.GetCurrentDynamicUniformDataBuffer()->UploadData(static_cast<void*>(&dynamicUniformData));
}