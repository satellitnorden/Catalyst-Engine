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
#include <Entities/TerrainEntity.h>
#include <Entities/VegetationEntity.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>
#include <Entities/InitializationData/TerrainInitializationData.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystBaseMath.h>
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
	_FrameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages(), _DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::DynamicUniformData)], _DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Environment)]
#if defined(CATALYST_ENABLE_OCEAN)
		, _DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Ocean)]
#endif
	);
}

/*
*	Pre-updates the Vulkan rendering system synchronously.
*/
void VulkanRenderingSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Pre-update the Vulkan interface.
	VulkanInterface::Instance->PreUpdate(_Semaphores[INDEX(GraphicsSemaphore::ImageAvailable)]);

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
	VulkanInterface::Instance->PostUpdate(_Semaphores[INDEX(GraphicsSemaphore::RenderFinished)]);
}

/*
*	Releases the Vulkan rendering system.
*/
void VulkanRenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release all descriptor set layouts.
	for (uint32 i = 0; i < INDEX(CommonRenderDataTableLayout::NumberOfCommonRenderDataTableLayouts); ++i)
	{
		_DescriptorSetLayouts[i].Release();
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
	return _RenderTargets[INDEX(renderTarget)];
}

/*
*	Returns the given uniform buffer.
*/
UniformBufferHandle VulkanRenderingSystem::GetUniformBuffer(const UniformBuffer uniformBuffer) NOEXCEPT
{
	return _UniformBuffers[INDEX(uniformBuffer)];
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
	//Put in a queue, destroy when no command buffer uses it anymore.
}

/*
*	Creates an environment material.
*/
void VulkanRenderingSystem::CreateEnvironmentMaterial(const EnvironmentMaterialData &environmentMaterialData, EnvironmentMaterial &environmentMaterial) NOEXCEPT
{
	//Create the diffuse texture.
	environmentMaterial._DiffuseTexture = static_cast<TextureCubeMapHandle>(VulkanInterface::Instance->CreateCubeMapTexture(environmentMaterialData._DiffuseData.Data(), environmentMaterialData._DiffuseResolution, environmentMaterialData._DiffuseResolution));

	//Create the diffuse irradiance texture.
	environmentMaterial._DiffuseIrradianceTexture = static_cast<TextureCubeMapHandle>(VulkanInterface::Instance->CreateCubeMapTexture(environmentMaterialData._DiffuseIrradianceData.Data(), environmentMaterialData._DiffuseIrradianceResolution, environmentMaterialData._DiffuseIrradianceResolution));
}

/*
*	Initializes a terrain entity.
*/
void VulkanRenderingSystem::InitializeTerrainEntity(const TerrainEntity *const RESTRICT entity, const TerrainInitializationData *const RESTRICT data) const NOEXCEPT
{
	//Fill the terrain entity components with the relevant data.
	FrustumCullingComponent &frustumCullingComponent{ ComponentManager::GetTerrainFrustumCullingComponents()[entity->_ComponentsIndex] };
	TerrainComponent &terrainComponent{ ComponentManager::GetTerrainTerrainComponents()[entity->_ComponentsIndex] };
	TerrainRenderComponent &renderComponent{ ComponentManager::GetTerrainTerrainRenderComponents()[entity->_ComponentsIndex] };

	frustumCullingComponent._ModelSpaceAxisAlignedBoundingBox = data->_AxisAlignedBoundingBox;

	Texture2DHandle terrainPropertiesTexture = Create2DTexture(TextureData(TextureDataContainer(data->_TerrainProperties), AddressMode::ClampToEdge, TextureFilter::Linear, MipmapMode::Nearest, TextureFormat::R32G32B32A32_Float));

	terrainComponent._TerrainUniformData = data->_TerrainUniformData;
	terrainComponent._UniformBuffer = VulkanInterface::Instance->CreateUniformBuffer(sizeof(TerrainUniformData));
	static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent._UniformBuffer)->UploadData(&terrainComponent._TerrainUniformData);
 	terrainComponent._TerrainProperties = data->_TerrainProperties;
	terrainComponent._TerrainPropertiesTexture = terrainPropertiesTexture;

	//Create the descriptor set.
	renderComponent._RenderDataTable = VulkanInterface::Instance->CreateDescriptorSet(_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Terrain)]);
	VulkanDescriptorSet& newDescriptorSet{ *static_cast<VulkanDescriptorSet *const RESTRICT>(renderComponent._RenderDataTable) };

	StaticArray<VkWriteDescriptorSet, 18> writeDescriptorSets
	{
		static_cast<const VulkanUniformBuffer *RESTRICT>(terrainComponent._UniformBuffer)->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(terrainPropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_LayerWeightsTexture)->GetWriteDescriptorSet(newDescriptorSet, 3),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FirstLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 4),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FirstLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 5),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FirstLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 6),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._SecondLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 7),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._SecondLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 8),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._SecondLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 9),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._ThirdLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 10),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._ThirdLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 11),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._ThirdLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 12),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FourthLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 13),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FourthLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 14),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FourthLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 15),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FifthLayerAlbedo)->GetWriteDescriptorSet(newDescriptorSet, 16),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FifthLayerNormalMap)->GetWriteDescriptorSet(newDescriptorSet, 17),
		static_cast<const Vulkan2DTexture *RESTRICT>(data->_TerrainMaterial._FifthLayerMaterialProperties)->GetWriteDescriptorSet(newDescriptorSet, 18)
	};

	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
}

/*
*	Initializes an instanced physical entity.
*/
void VulkanRenderingSystem::InitializeInstancedPhysicalEntity(const InstancedPhysicalEntity &entity, const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) const NOEXCEPT
{
	//Cache relevant data.
	const PhysicalMaterial material{ RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Red) };
	InstancedPhysicalRenderComponent &renderComponent{ ComponentManager::GetInstancedPhysicalInstancedPhysicalRenderComponents()[entity._ComponentsIndex] };

	//Create the transformations buffer.
	const void *RESTRICT transformationsData[]{ transformations.Data() };
	const VkDeviceSize transformationsDataSizes[]{ sizeof(Matrix4) * transformations.Size() };
	VulkanConstantBuffer *RESTRICT transformationsBuffer = VulkanInterface::Instance->CreateConstantBuffer(transformationsData, transformationsDataSizes, 1);

	//Fill the instanced physical entity components with the relevant data.
	renderComponent._ModelBuffer = model._Buffer;
	renderComponent._RenderDataTable = material._RenderDataTable;
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
	VegetationComponent &renderComponent{ ComponentManager::GetVegetationVegetationComponents()[entity._ComponentsIndex] };
	VegetationCullingComponent &cullingComponent{ ComponentManager::GetVegetationVegetationCullingComponents()[entity._ComponentsIndex] };

	//Calculate the vegetation grid.
	DynamicArray<VegetationTransformation> sortedTransformations;
	RenderingUtilities::CalculateVegetationGrid(properties._CutoffDistance, transformations, &renderComponent, &cullingComponent, sortedTransformations);

	//The cutoff distance is used as a squared value in the shadres, so square it.
	VegetationProperties shaderProperties{ properties };
	shaderProperties._CutoffDistance *= shaderProperties._CutoffDistance;

	//Create the vegetation properties uniform buffer.
	VulkanUniformBuffer *const RESTRICT propertiesBuffer{ VulkanInterface::Instance->CreateUniformBuffer(sizeof(VegetationProperties)) };
	propertiesBuffer->UploadData(&shaderProperties);

	//Create the descriptor set.
	renderComponent._RenderDataTable = VulkanInterface::Instance->CreateDescriptorSet(_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Vegetation)]);
	VulkanDescriptorSet& newDescriptorSet{ *static_cast<VulkanDescriptorSet *const RESTRICT>(renderComponent._RenderDataTable) };

	//Update the write descriptor sets.
	StaticArray<VkWriteDescriptorSet, 5> writeDescriptorSets
	{
		propertiesBuffer->GetWriteDescriptorSet(newDescriptorSet, 1),
		static_cast<const Vulkan2DTexture *RESTRICT>(material._MaskTexture)->GetWriteDescriptorSet(newDescriptorSet, 2),
		static_cast<const Vulkan2DTexture *RESTRICT>(material._AlbedoTexture)->GetWriteDescriptorSet(newDescriptorSet, 3),
		static_cast<const Vulkan2DTexture *RESTRICT>(material._NormalMapTexture)->GetWriteDescriptorSet(newDescriptorSet, 4),
		static_cast<const Vulkan2DTexture *RESTRICT>(material._PropertiesTexture)->GetWriteDescriptorSet(newDescriptorSet, 5)
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
*	Creates and returns a 2D texture given the texture data.
*/
Texture2DHandle VulkanRenderingSystem::Create2DTexture(const TextureData &textureData) const NOEXCEPT
{
	//Create the Vulkan 2D texture.
	Vulkan2DTexture *RESTRICT new2DTexture = VulkanInterface::Instance->Create2DTexture(static_cast<uint32>(textureData._TextureDataContainer._TextureData.Size()), textureData._TextureDataContainer._TextureWidth, textureData._TextureDataContainer._TextureHeight, textureData._TextureDataContainer._TextureChannels, textureData._TextureDataContainer._TextureTexelSize, textureData._TextureDataContainer._TextureData.Data(), VulkanTranslationUtilities::GetVulkanFormat(textureData._TextureFormat), VulkanTranslationUtilities::GetVulkanTextureFilter(textureData._MagnificationFilter), VulkanTranslationUtilities::GetVulkanMipmapMode(textureData._MipmapMode), VulkanTranslationUtilities::GetVulkanAddressMode(textureData._AddressMode));

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
*	Uploads data to a uniform buffer.
*/
void VulkanRenderingSystem::UploadDataToUniformBuffer(UniformBufferHandle handle, const void *const RESTRICT data) const NOEXCEPT
{
	static_cast<VulkanUniformBuffer *const RESTRICT>(handle)->UploadData(data);
}

/*
*	Destroys a uniform buffer.
*/
void VulkanRenderingSystem::DestroyUniformBuffer(UniformBufferHandle handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
}

/*
*	Finalizes the initialization of a render pass.
*/
void VulkanRenderingSystem::FinalizeRenderPassInitialization(RenderPass *const RESTRICT _RenderPass) NOEXCEPT
{
	//Create the pipeline.
	VulkanPipelineCreationParameters parameters;

	parameters._BlendEnable = _RenderPass->IsBlendEnabled();
	parameters._ColorAttachmentCount = static_cast<uint32>(_RenderPass->GetRenderTargets().Size());
	parameters._CullMode = VulkanTranslationUtilities::GetVulkanCullMode(_RenderPass->GetCullMode());
	parameters._DepthCompareOp = VulkanTranslationUtilities::GetVulkanCompareOperator(_RenderPass->GetDepthCompareOperator());
	parameters._DepthTestEnable = _RenderPass->IsDepthTestEnabled();
	parameters._DepthWriteEnable = _RenderPass->IsDepthWriteEnabled();

	DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
	pipelineDescriptorSetLayouts.Reserve(_RenderPass->GetRenderDataTableLayouts().Size());

	for (RenderDataTableLayoutHandle renderDataTableLayout : _RenderPass->GetRenderDataTableLayouts())
	{
		pipelineDescriptorSetLayouts.EmplaceFast(*static_cast<VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
	}

	parameters._DescriptorSetLayoutCount = static_cast<uint32>(pipelineDescriptorSetLayouts.Size());
	parameters._DescriptorSetLayouts = pipelineDescriptorSetLayouts.Data();

	DynamicArray<VkPushConstantRange> pushConstantRanges;

	if (_RenderPass->GetPushConstantRanges().Empty())
	{
		parameters._PushConstantRangeCount = 0;
		parameters._PushConstantRanges = nullptr;
	}

	else
	{
		pushConstantRanges.Reserve(_RenderPass->GetPushConstantRanges().Size());

		for (const PushConstantRange &pushConstantRange : _RenderPass->GetPushConstantRanges())
		{
			pushConstantRanges.EmplaceFast(VulkanTranslationUtilities::GetVulkanPushConstantRange(pushConstantRange));
		}

		parameters._PushConstantRangeCount = static_cast<uint32>(pushConstantRanges.Size());
		parameters._PushConstantRanges = pushConstantRanges.Data();
	}
	
	parameters._ShaderModules.Reserve(5);
	if (_RenderPass->GetVertexShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[INDEX(_RenderPass->GetVertexShader())]);
	if (_RenderPass->GetTessellationControlShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[INDEX(_RenderPass->GetTessellationControlShader())]);
	if (_RenderPass->GetTessellationEvaluationShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[INDEX(_RenderPass->GetTessellationEvaluationShader())]);
	if (_RenderPass->GetGeometryShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[INDEX(_RenderPass->GetGeometryShader())]);
	if (_RenderPass->GetFragmentShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[INDEX(_RenderPass->GetFragmentShader())]);
	parameters._Subpass = _RenderPass->GetSubStageIndex();
	parameters._Topology = VulkanTranslationUtilities::GetVulkanTopology(_RenderPass->GetTopology());

	DynamicArray<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	vertexInputAttributeDescriptions.Reserve(_RenderPass->GetVertexInputAttributeDescriptions().Size());

	for (const VertexInputAttributeDescription &vertexInputAttributeDescription : _RenderPass->GetVertexInputAttributeDescriptions())
	{
		vertexInputAttributeDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputAttributeDescription(vertexInputAttributeDescription));
	}

	parameters._VertexInputAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.Size());
	parameters._VertexInputAttributeDescriptions = vertexInputAttributeDescriptions.Data();

	DynamicArray<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	vertexInputBindingDescriptions.Reserve(_RenderPass->GetVertexInputBindingDescriptions().Size());

	for (const VertexInputBindingDescription &vertexInputBindingDescription : _RenderPass->GetVertexInputBindingDescriptions())
	{
		vertexInputBindingDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputBindingDescription(vertexInputBindingDescription));
	}

	parameters._VertexInputBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.Size());
	parameters._VertexInputBindingDescriptions = vertexInputBindingDescriptions.Data();
	parameters._ViewportExtent = _RenderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ _RenderPass->GetRenderResolution()._Width, _RenderPass->GetRenderResolution()._Height };

	parameters._RenderPass = _VulkanRenderPassMainStageData[INDEX(_RenderPass->GetMainStage())]._RenderPass;

	//Create the pipeline!
	_Pipelines[INDEX(_RenderPass->GetSubStage())] = VulkanInterface::Instance->CreatePipeline(parameters);

	//Update the Vulkan render pass data.
	_VulkanRenderPassData[INDEX(_RenderPass->GetSubStage())]._Framebuffers.Reserve(_VulkanRenderPassMainStageData[INDEX(_RenderPass->GetMainStage())]._FrameBuffers.Size());

	for (VulkanFramebuffer *RESTRICT vulkanFrameBuffer : _VulkanRenderPassMainStageData[INDEX(_RenderPass->GetMainStage())]._FrameBuffers)
	{
		_VulkanRenderPassData[INDEX(_RenderPass->GetSubStage())]._Framebuffers.EmplaceFast(vulkanFrameBuffer->Get());
	}

	_VulkanRenderPassData[INDEX(_RenderPass->GetSubStage())]._Pipeline = _Pipelines[INDEX(_RenderPass->GetSubStage())]->Get();
	_VulkanRenderPassData[INDEX(_RenderPass->GetSubStage())]._PipelineLayout = _Pipelines[INDEX(_RenderPass->GetSubStage())]->GetPipelineLayout();
	_VulkanRenderPassData[INDEX(_RenderPass->GetSubStage())]._RenderPass = _VulkanRenderPassMainStageData[INDEX(_RenderPass->GetMainStage())]._RenderPass->Get();

	_RenderPass->SetData(&_VulkanRenderPassData[INDEX(_RenderPass->GetSubStage())]);

	//Add the command buffers.
	const uint64 numberOfCommandBuffers{ VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() };
	_RenderPass->SetNumberOfCommandBuffers(numberOfCommandBuffers);

	//Create the directional shadow command pool.
	VulkanCommandPool *const RESTRICT pipelineCommandPool{ VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) };

	for (uint64 i = 0; i < numberOfCommandBuffers; ++i)
	{
		VulkanCommandBuffer pipelineCommandBuffer;
		pipelineCommandPool->AllocateSecondaryCommandBuffer(pipelineCommandBuffer);
		_RenderPass->AddCommandBuffer(new VulkanTranslationCommandBuffer(pipelineCommandBuffer));
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

		vulkanBindings.EmplaceFast(VulkanUtilities::CreateDescriptorSetLayoutBinding(binding._Binding, VulkanTranslationUtilities::GetVulkanDescriptorType(binding._Type), VulkanTranslationUtilities::GetVulkanShaderStage(binding._ShaderStage)));
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

	switch (information._Type)
	{
		case RenderDataTableUpdateInformation::Type::RenderTarget:
		{
			writeDescriptorSet = static_cast<VulkanRenderTarget *const RESTRICT>(information._Handle)->GetWriteDescriptorSet(*descriptorSet, information._Binding);

			break;
		}

		case RenderDataTableUpdateInformation::Type::Texture2D:
		{
			writeDescriptorSet = static_cast<Vulkan2DTexture *const RESTRICT>(information._Handle)->GetWriteDescriptorSet(*descriptorSet, information._Binding);

			break;
		}

		case RenderDataTableUpdateInformation::Type::UniformBuffer:
		{
			writeDescriptorSet = static_cast<VulkanUniformBuffer *const RESTRICT>(information._Handle)->GetWriteDescriptorSet(*descriptorSet, information._Binding);

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
	return reinterpret_cast<RenderDataTableHandle>(_FrameData.GetCurrentDynamicUniformDataRenderDataTable());
}

/*
*	Returns the current environment data descriptor set.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCurrentEnvironmentRenderDataTable() NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(_FrameData.GetCurrentEnvironmentDescriptorSet());
}

#if defined(CATALYST_ENABLE_OCEAN)
/*
*	Returns the current ocean descriptor set.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCurrentOceanRenderDataTable() NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(_FrameData.GetCurrentOceanRenderDataTable());
}
#endif

/*
*	Returns the given common render data table layout.
*/
RenderDataTableHandle VulkanRenderingSystem::GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) NOEXCEPT
{
	return reinterpret_cast<RenderDataTableHandle>(&_DescriptorSetLayouts[INDEX(commonRenderDataTableLayout)]);
}

/*
*	Initializes all render targets.
*/
void VulkanRenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Get the scaled extent.
	const VkExtent2D scaledExtent{ VulkanTranslationUtilities::GetVulkanExtent(RenderingSystem::Instance->GetScaledResolution()) };

	//Initialize all depth buffers.
	_DepthBuffers[INDEX(DepthBuffer::DirectionalLight)] = VulkanInterface::Instance->CreateDepthBuffer({ EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution });
	_DepthBuffers[INDEX(DepthBuffer::SceneBuffer)] = VulkanInterface::Instance->CreateDepthBuffer(scaledExtent);

	//Initialize all render targets.
	_RenderTargets[INDEX(RenderTarget::DirectionalShadowMap)] = VulkanInterface::Instance->CreateRenderTarget({ EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution }, VK_FORMAT_R8_UNORM, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	_RenderTargets[INDEX(RenderTarget::DirectionalShadow)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R8_UNORM, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
	_RenderTargets[INDEX(RenderTarget::SceneBufferAlbedo)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R8G8B8A8_SNORM, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	_RenderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	_RenderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R8G8B8A8_SNORM, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	_RenderTargets[INDEX(RenderTarget::SceneIntermediate)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	_RenderTargets[INDEX(RenderTarget::Scene)] = VulkanInterface::Instance->CreateRenderTarget(scaledExtent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
}

/*
*	Initializes all semaphores.
*/
void VulkanRenderingSystem::InitializeSemaphores() NOEXCEPT
{
	//Initialize all semaphores.
	_Semaphores[INDEX(GraphicsSemaphore::ImageAvailable)] = VulkanInterface::Instance->CreateSemaphore();
	_Semaphores[INDEX(GraphicsSemaphore::RenderFinished)] = VulkanInterface::Instance->CreateSemaphore();
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
			sample._X = CatalystBaseMath::RandomFloatInRange(-1.0f, 1.0f);
			sample._Y = CatalystBaseMath::RandomFloatInRange(-1.0f, 1.0f);
			sample._Z = CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f);
			sample._W = 0.0f;

			sample.Normalize();

			float scale{ CatalystBaseMath::RandomFloatInRange(0.0f, 1.0f) };
			scale = CatalystBaseMath::LinearlyInterpolate(0.1f, 1.0f, scale * scale);

			sample *= scale;
		}

		_UniformBuffers[INDEX(UniformBuffer::ScreenSpaceAmbientOcclusionSamples)] = VulkanInterface::Instance->CreateUniformBuffer(sizeof(Vector4) * RenderingConstants::SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLE_KERNEL_SIZE);
		_UniformBuffers[INDEX(UniformBuffer::ScreenSpaceAmbientOcclusionSamples)]->UploadData(samples.Data());
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

		_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::DynamicUniformData)].Initialize(1, dynamicUniformDataDescriptorSetLayoutBindings.Data());
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

		_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Environment)].Initialize(static_cast<uint32>(environmentDescriptorSetLayoutBindings.Size()), environmentDescriptorSetLayoutBindings.Data());
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

		_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Terrain)].Initialize(18, terrainDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the physical descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 4> physicalDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Physical)].Initialize(static_cast<uint32>(physicalDescriptorSetLayoutBindings.Size()), physicalDescriptorSetLayoutBindings.Data());
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

		_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Vegetation)].Initialize(static_cast<uint32>(vegetationDescriptorSetLayoutBindings.Size()), vegetationDescriptorSetLayoutBindings.Data());
	}

#if defined(CATALYST_ENABLE_OCEAN)
	{
		//Initialize the ocean descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 3> oceanDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::Ocean)].Initialize(static_cast<uint32>(oceanDescriptorSetLayoutBindings.Size()), oceanDescriptorSetLayoutBindings.Data());
	}
#endif

	{
		//Initialize the particle system descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 2> particleSystemDescriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT),
			VulkanUtilities::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::ParticleSystem)].Initialize(static_cast<uint32>(particleSystemDescriptorSetLayoutBindings.Size()), particleSystemDescriptorSetLayoutBindings.Data());
	}

	{
		//Initialize the bloom descriptor set layout.
		constexpr StaticArray<VkDescriptorSetLayoutBinding, 1> descriptorSetLayoutBindings
		{
			VulkanUtilities::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		};

		_DescriptorSetLayouts[INDEX(CommonRenderDataTableLayout::GaussianBlur)].Initialize(static_cast<uint32>(descriptorSetLayoutBindings.Size()), descriptorSetLayoutBindings.Data());
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
		_ShaderModules[INDEX(Shader::BloomFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the box blur shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBoxBlurFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::BoxBlurFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the cube map vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetCubeMapVertexShaderData(data);
		_ShaderModules[INDEX(Shader::CubeMapVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

#if !defined(CATALYST_FINAL)
	{
		//Initialize the debug axis-aligned bounding box fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDebugAxisAlignedBoundingBoxFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::DebugAxisAlignedBoundingBoxFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the debug axis-aligned bounding box vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDebugAxisAlignedBoundingBoxVertexShaderData(data);
		_ShaderModules[INDEX(Shader::DebugAxisAlignedBoundingBoxVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the debug screen box fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDebugScreenBoxFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::DebugScreenBoxFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the debug screen box vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDebugScreenBoxVertexShaderData(data);
		_ShaderModules[INDEX(Shader::DebugScreenBoxVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
#endif

	{
		//Initialize the directional physical shadow vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalPhysicalShadowVertexShaderData(data);
		_ShaderModules[INDEX(Shader::DirectionalPhysicalShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the directional shadow fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::DirectionalShadowFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize directional shadow instanced physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowInstancedPhysicalVertexShaderData(data);
		_ShaderModules[INDEX(Shader::DirectionalInstancedPhysicalShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowTerrainTessellationEvaluationShaderData(data);
		_ShaderModules[INDEX(Shader::DirectionalTerrainShadowTessellationEvaluation)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetGaussianBlurFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::GaussianBlurFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the instanced physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetInstancedPhysicalVertexShaderData(data);
		_ShaderModules[INDEX(Shader::InstancedPhysicalVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the lighting fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLightingFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::LightingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

#if defined(CATALYST_ENABLE_OCEAN)
	{
		//Initialize the ocean fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetOceanFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::OceanFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
#endif

	{
		//Initialize the outline fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetOutlineFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::OutlineFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the particle system fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::ParticleSystemFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the particle system geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemGeometryShaderData(data);
		_ShaderModules[INDEX(Shader::ParticleSystemGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the particle system vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemVertexShaderData(data);
		_ShaderModules[INDEX(Shader::ParticleSystemVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the post processing bloom fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPostProcessingBloomFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::PostProcessingBloomFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the post processing fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPostProcessingFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::PostProcessingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPhysicalFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::PhysicalFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPhysicalVertexShaderData(data);
		_ShaderModules[INDEX(Shader::PhysicalVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the screen space ambient occlusion fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetScreenSpaceAmbientOcclusionFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::ScreenSpaceAmbientOcclusionFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the shadow map fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetShadowMapFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::ShadowMapFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the sky fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetSkyFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::SkyFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the terrain scene buffer fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::TerrainFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the terrain scene buffer tessellation control shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferTessellationControlShaderData(data);
		_ShaderModules[INDEX(Shader::TerrainTessellationControl)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
	}
	
	{
		//Initialize the terrain scene buffer tessellation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainSceneBufferTessellationEvaluationShaderData(data);
		_ShaderModules[INDEX(Shader::TerrainTessellationEvaluation)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}
	
	{
		//Initialize the terrain scene buffer vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainVertexShaderData(data);
		_ShaderModules[INDEX(Shader::TerrainVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::VegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the vegetation geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationGeometryShaderData(data);
		_ShaderModules[INDEX(Shader::VegetationGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}
	
	{
		//Initialize the vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationVertexShaderData(data);
		_ShaderModules[INDEX(Shader::VegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the viewport vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetViewportVertexShaderData(data);
		_ShaderModules[INDEX(Shader::ViewportVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	{
		//Initialize the volumetric fog fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVolumetricFogFragmentShaderData(data);
		_ShaderModules[INDEX(Shader::VolumetricFogFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
#endif
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
			VulkanUtilities::CreateAttachmentDescription(	_DepthBuffers[INDEX(DepthBuffer::DirectionalLight)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),

			//Shadow map.
			VulkanUtilities::CreateAttachmentDescription(	_RenderTargets[INDEX(RenderTarget::DirectionalShadowMap)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

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

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		StaticArray<VkSubpassDependency, NUMBER_OF_DIRECTIONAL_SHADOW_SUBPASSES - 1> subpassDependencies
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

		renderPassParameters._SubpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters._SubpassDependencies = subpassDependencies.Data();

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)]._RenderPass->Get();
		
		StaticArray<VkImageView, 2> attachments
		{
			_DepthBuffers[INDEX(DepthBuffer::DirectionalLight)]->GetImageView(),
			_RenderTargets[INDEX(RenderTarget::DirectionalShadowMap)]->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution };

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)]._FrameBuffers.EmplaceFast( VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)]._NumberOfAttachments = 2;
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::DirectionalShadow)]._ShouldClear = true;
	}

	//Initialize the scene buffer render pass.
	{
		constexpr uint64 NUMBER_OF_SCENE_BUFFER_SUBPASSES{ 9
#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
		+ 1
#endif
		};

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
			VulkanUtilities::CreateAttachmentDescription(	_DepthBuffers[INDEX(DepthBuffer::SceneBuffer)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),
			
			//Albedo.
			VulkanUtilities::CreateAttachmentDescription(	_RenderTargets[INDEX(RenderTarget::SceneBufferAlbedo)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Normal depth.
			VulkanUtilities::CreateAttachmentDescription(	_RenderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Material properties.
			VulkanUtilities::CreateAttachmentDescription(	_RenderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Directional shadow.
			VulkanUtilities::CreateAttachmentDescription(	_RenderTargets[INDEX(RenderTarget::DirectionalShadow)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	_RenderTargets[INDEX(RenderTarget::Scene)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

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

		constexpr VkAttachmentReference normalDepthInputAttachmentReference{ VulkanUtilities::CreateAttachmentReference(NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) };

		constexpr VkAttachmentReference directionalShadowColorAttachmentReference{ VulkanUtilities::CreateAttachmentReference(DIRECTIONAL_SHADOW_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) };

		subpassDescriptions[4] = VulkanUtilities::CreateSubpassDescription(	1,
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

		subpassDescriptions[5] = VulkanUtilities::CreateSubpassDescription(	static_cast<uint32>(lightingInputAttachmentReferences.Size()),
																			lightingInputAttachmentReferences.Data(),
																			1,
																			&sceneColorAttachmentReference,
																			nullptr,
																			0,
																			nullptr);

		subpassDescriptions[6] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			1,
																			&sceneColorAttachmentReference,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[7] = VulkanUtilities::CreateSubpassDescription(	0,
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

		subpassDescriptions[8] = VulkanUtilities::CreateSubpassDescription(	static_cast<uint32>(particleSystemAttachmentReferences.Size()),
																			particleSystemAttachmentReferences.Data(),
																			static_cast<uint32>(particleSystemAttachmentReferences.Size()),
																			particleSystemAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
		subpassDescriptions[9] = VulkanUtilities::CreateSubpassDescription(	1,
																				&normalDepthInputAttachmentReference,
																				1,
																				&sceneColorAttachmentReference,
																				nullptr,
																				0,
																				nullptr);
#endif

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

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
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
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
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
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

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
			VulkanUtilities::CreateSubpassDependency(	8,
														9,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),
#endif
		};

		renderPassParameters._SubpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters._SubpassDependencies = subpassDependencies.Data();

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)]._RenderPass->Get();

		StaticArray<VkImageView, 6> attachments
		{
			_DepthBuffers[INDEX(DepthBuffer::SceneBuffer)]->GetImageView(),
			_RenderTargets[INDEX(RenderTarget::SceneBufferAlbedo)]->GetImageView(),
			_RenderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetImageView(),
			_RenderTargets[INDEX(RenderTarget::SceneBufferMaterialProperties)]->GetImageView(),
			_RenderTargets[INDEX(RenderTarget::DirectionalShadow)]->GetImageView(),
			_RenderTargets[INDEX(RenderTarget::Scene)]->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)]._NumberOfAttachments = 6;
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Scene)]._ShouldClear = true;
	}

#if !defined(CATALYST_FINAL)
	//Initialize the debug render pass.
	{
		constexpr uint64 NUMBER_OF_DEBUG_SUBPASSES{ 2 };

		constexpr uint32 DEPTH_BUFFER_INDEX{ 0 };
		constexpr uint32 SCENE_INDEX{ 1 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 2> attachmenDescriptions
		{
			//Depth buffer.
			VulkanUtilities::CreateAttachmentDescription(	_DepthBuffers[INDEX(DepthBuffer::SceneBuffer)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),

			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	_RenderTargets[INDEX(RenderTarget::Scene)]->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr VkAttachmentReference depthAttachmentReference{ DEPTH_BUFFER_INDEX, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
		};

		StaticArray<VkSubpassDescription, NUMBER_OF_DEBUG_SUBPASSES> subpassDescriptions;

		subpassDescriptions[0] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			1,
																			colorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[1] = VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			1,
																			colorAttachmentReferences.Data(),
																			nullptr,
																			0,
																			nullptr);

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		StaticArray<VkSubpassDependency, NUMBER_OF_DEBUG_SUBPASSES - 1> subpassDependencies
		{
			VulkanUtilities::CreateSubpassDependency(	0,
														1,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_DEPENDENCY_BY_REGION_BIT)
		};

		renderPassParameters._SubpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters._SubpassDependencies = subpassDependencies.Data();;

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Debug)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Debug)]._RenderPass->Get();

		StaticArray<VkImageView, 2> attachments
		{
			_DepthBuffers[INDEX(DepthBuffer::SceneBuffer)]->GetImageView(),
			_RenderTargets[INDEX(RenderTarget::Scene)]->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Debug)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Debug)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Debug)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Debug)]._ShouldClear = false;
	}
#endif

#if defined(CATALYST_ENABLE_OCEAN)
	//Initialize the ocean render pass.
	{
		constexpr uint64 NUMBER_OF_OCEAN_SUBPASSES{ 1 };

		constexpr uint32 SCENE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Scene.
			VulkanUtilities::CreateAttachmentDescription(_RenderTargets[INDEX(RenderTarget::Scene)]->GetFormat(),
			VK_ATTACHMENT_LOAD_OP_LOAD,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

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

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		renderPassParameters._SubpassDependencyCount = 0;
		renderPassParameters._SubpassDependencies = nullptr;

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			_RenderTargets[INDEX(RenderTarget::Scene)]->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::Ocean)]._ShouldClear = false;
	}
#endif

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

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

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

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		renderPassParameters._SubpassDependencyCount = 0;
		renderPassParameters._SubpassDependencies = nullptr;

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffers.
		const DynamicArray<VkImageView> &swapchainImages{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)]._FrameBuffers.Reserve(swapchainImages.Size());

		for (VkImageView swapchainImage : swapchainImages)
		{
			VulkanFramebufferCreationParameters framebufferParameters;

			framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)]._RenderPass->Get();

			framebufferParameters._AttachmentCount = 1;
			framebufferParameters._Attachments = &swapchainImage;
			framebufferParameters._Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

			_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		}

		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[INDEX(RenderPassMainStage::PostProcessingFinal)]._ShouldClear = false;
	}
}

/*
*	Begins the frame.
*/
void VulkanRenderingSystem::BeginFrame() NOEXCEPT
{
	//Set the current frame.
	_FrameData.SetCurrentFrame(VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());

	//Set the current dynamic uniform data descriptor set.
	_CurrentDynamicUniformDataDescriptorSet = _FrameData.GetCurrentDynamicUniformDataRenderDataTable();

	//Set the current environment descriptor set.
	_CurrentEnvironmentDataDescriptorSet = _FrameData.GetCurrentEnvironmentDescriptorSet();

	//Wait for the current fence to finish.
	_FrameData.GetCurrentFence()->WaitFor();

	//Reset the current fence.
	_FrameData.GetCurrentFence()->Reset();
}

/*
*	Concatenates all secondary command buffers into the previous one.
*/
void VulkanRenderingSystem::ConcatenateCommandBuffers() NOEXCEPT
{
	//Begin the current primary command buffer.
	VulkanCommandBuffer *const RESTRICT currentPrimaryCommandBuffer{ _FrameData.GetCurrentPrimaryCommandBuffer() };
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

#if defined(CATALYST_ENABLE_OCEAN)
			//Specialization - Copy scene to scene intermediate for the ocean render pass.
			if (currentStage == RenderPassMainStage::Ocean)
			{
				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
																		VK_ACCESS_TRANSFER_READ_BIT,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
																		_RenderTargets[INDEX(RenderTarget::Scene)]->GetImage(),
																		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	0,
																		VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
																		_RenderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetImage(),
																		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				const VkExtent2D sourceExtent{ _RenderTargets[INDEX(RenderTarget::Scene)]->GetExtent() };
				const VkExtent3D extent{ sourceExtent.width, sourceExtent.height, 0 };

				currentPrimaryCommandBuffer->CommandCopyImage(_RenderTargets[INDEX(RenderTarget::Scene)]->GetImage(), _RenderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetImage(), extent);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
																		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		_RenderTargets[INDEX(RenderTarget::Scene)]->GetImage(),
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_ACCESS_SHADER_READ_BIT,
																		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		_RenderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetImage(),
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);
			}
#endif

			if (_VulkanRenderPassMainStageData[INDEX(currentStage)]._ShouldClear)
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear(	_VulkanRenderPassMainStageData[INDEX(currentStage)]._RenderPass->Get(),
																				_VulkanRenderPassMainStageData[INDEX(currentStage)]._FrameBuffers[renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameIndex() : 0]->Get(),
																				renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution()._Width, renderPass->GetRenderResolution()._Height },
																				VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, _VulkanRenderPassMainStageData[INDEX(currentStage)]._NumberOfAttachments);
			}

			else
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPass(	_VulkanRenderPassMainStageData[INDEX(currentStage)]._RenderPass->Get(),
																		_VulkanRenderPassMainStageData[INDEX(currentStage)]._FrameBuffers[renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameIndex() : 0]->Get(),
																		renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution()._Width, renderPass->GetRenderResolution()._Height },
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
	_FrameData.GetCurrentPrimaryCommandBuffer()->End();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue()->Submit(*_FrameData.GetCurrentPrimaryCommandBuffer(), 1, _Semaphores[INDEX(GraphicsSemaphore::ImageAvailable)], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 1, _Semaphores[INDEX(GraphicsSemaphore::RenderFinished)], _FrameData.GetCurrentFence()->Get());
}

/*
*	Updates the descriptor sets.
*/
void VulkanRenderingSystem::UpdateDescriptorSets() NOEXCEPT
{
	{
		//Update the environment descriptor set.
		VulkanDescriptorSet &environmentDescriptorSet{ *_CurrentEnvironmentDataDescriptorSet };

		StaticArray<VkWriteDescriptorSet, 4> environmentWriteDescriptorSets
		{
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetNightEnvironmentMaterial()._DiffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 0),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetNightEnvironmentMaterial()._DiffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 1),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetDayEnvironmentMaterial()._DiffuseTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 2),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetDayEnvironmentMaterial()._DiffuseIrradianceTexture)->GetWriteDescriptorSet(environmentDescriptorSet, 3)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(environmentWriteDescriptorSets.Size()), environmentWriteDescriptorSets.Data(), 0, nullptr);
	}

#if defined(CATALYST_ENABLE_OCEAN)
	{
		//Update the ocean descriptor set.
		VulkanDescriptorSet &oceanDescriptorSet{ *_FrameData.GetCurrentOceanRenderDataTable() };

		StaticArray<VkWriteDescriptorSet, 3> oceanWriteDescriptorSets
		{
			_RenderTargets[INDEX(RenderTarget::SceneIntermediate)]->GetWriteDescriptorSet(oceanDescriptorSet, 0),
			_RenderTargets[INDEX(RenderTarget::SceneBufferNormalDepth)]->GetWriteDescriptorSet(oceanDescriptorSet, 1),
			static_cast<const VulkanCubeMapTexture *const RESTRICT>(EnvironmentManager::Instance->GetOceanMaterial()._NormalMapTexture)->GetWriteDescriptorSet(oceanDescriptorSet, 2)
		};

		vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(oceanWriteDescriptorSets.Size()), oceanWriteDescriptorSets.Data(), 0, nullptr);
	}
#endif
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
	const Matrix4 *const RESTRICT inverseProjectionMatrix{ RenderingSystem::Instance->GetInverseProjectionMatrix() };
	const Matrix4 *const RESTRICT inverseCameraMatrix{ RenderingSystem::Instance->GetInverseCameraMatrix() };

	Matrix4 cameraOriginMatrix{ *cameraMatrix };
	cameraOriginMatrix.SetTranslation(Vector3(0.0f, 0.0f, 0.0f));

	_DynamicUniformData._CameraFieldOfViewCosine = CatalystBaseMath::CosineDegrees(activeCamera->GetFieldOfView()) - 0.2f;
	_DynamicUniformData._InverseCameraMatrix = *inverseCameraMatrix;
	_DynamicUniformData._InverseProjectionMatrix = *inverseProjectionMatrix;
	_DynamicUniformData._OriginViewMatrix = *projectionMatrix * cameraOriginMatrix;
	_DynamicUniformData._ViewMatrix = *viewMatrix;
	_DynamicUniformData._CameraForwardVector = forwardVector;
	_DynamicUniformData._CameraWorldPosition = cameraWorldPosition;

	const uint64 numberOfDirectionalLightEntityComponents{ ComponentManager::GetNumberOfDirectionalLightComponents() };

	if (numberOfDirectionalLightEntityComponents > 0)
	{
		const DirectionalLightComponent *RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightDirectionalLightComponents() };

		_DynamicUniformData._DirectionalLightIntensity = directionalLightComponent->_Intensity;
		_DynamicUniformData._DirectionalLightViewMatrix = RenderingUtilities::CalculateDirectionalLightViewMatrix();
		_DynamicUniformData._DirectionalLightDirection = Vector3(0.0f, 0.0f, -1.0f).Rotated(directionalLightComponent->_Rotation);
		_DynamicUniformData._DirectionalLightColor = directionalLightComponent->_Color;
		_DynamicUniformData._DirectionalLightScreenSpacePosition = *viewMatrix * Vector4(-_DynamicUniformData._DirectionalLightDirection._X * 100.0f + cameraWorldPosition._X, -_DynamicUniformData._DirectionalLightDirection._Y * 100.0f + cameraWorldPosition._Y, -_DynamicUniformData._DirectionalLightDirection._Z * 100.0f + cameraWorldPosition._Z, 1.0f);
		_DynamicUniformData._DirectionalLightScreenSpacePosition._X /= _DynamicUniformData._DirectionalLightScreenSpacePosition._W;
		_DynamicUniformData._DirectionalLightScreenSpacePosition._Y /= _DynamicUniformData._DirectionalLightScreenSpacePosition._W;
		_DynamicUniformData._DirectionalLightScreenSpacePosition._Z /= _DynamicUniformData._DirectionalLightScreenSpacePosition._W;
	}

	else
	{
		_DynamicUniformData._DirectionalLightIntensity = 0.0f;
		_DynamicUniformData._DirectionalLightViewMatrix = Matrix4();
		_DynamicUniformData._DirectionalLightDirection = Vector3(0.0f, 0.0f, 0.0f);
		_DynamicUniformData._DirectionalLightColor = Vector3(0.0f, 0.0f, 0.0f);
		_DynamicUniformData._DirectionalLightScreenSpacePosition = Vector3(0.0f, 0.0f, 0.0f);
	}

	_DynamicUniformData._EnvironmentBlend = EnvironmentManager::Instance->GetEnvironmentBlend();

	_DynamicUniformData._DeltaTime = EngineSystem::Instance->GetDeltaTime();
	_DynamicUniformData._TotalGameTime = EngineSystem::Instance->GetTotalTime();

	uint64 counter = 0;

	const uint64 numberOfPointLightEntityComponents{ ComponentManager::GetNumberOfPointLightComponents() };
	const PointLightComponent *RESTRICT pointLightComponent{ ComponentManager::GetPointLightPointLightComponents() };

	_DynamicUniformData._NumberOfPointLights = static_cast<int32>(numberOfPointLightEntityComponents);

	for (uint64 i = 0; i < numberOfPointLightEntityComponents; ++i, ++pointLightComponent)
	{
		if (!pointLightComponent->_Enabled)
		{
			--_DynamicUniformData._NumberOfPointLights;

			continue;
		}

		_DynamicUniformData._PointLightAttenuationDistances[counter] = pointLightComponent->_AttenuationDistance;
		_DynamicUniformData._PointLightIntensities[counter] = pointLightComponent->_Intensity;
		_DynamicUniformData._PointLightColors[counter] = pointLightComponent->_Color;
		_DynamicUniformData._PointLightWorldPositions[counter] = pointLightComponent->_Position;

		++counter;
	}

	counter = 0;

	const uint64 numberOfSpotLightEntityComponents{ ComponentManager::GetNumberOfSpotLightComponents() };
	const SpotLightComponent *RESTRICT spotLightComponent{ ComponentManager::GetSpotLightSpotLightComponents() };

	_DynamicUniformData._NumberOfSpotLights = static_cast<int32>(numberOfSpotLightEntityComponents);

	for (uint64 i = 0; i < numberOfSpotLightEntityComponents; ++i, ++spotLightComponent)
	{
		if (!spotLightComponent->_Enabled)
		{
			--_DynamicUniformData._NumberOfSpotLights;

			continue;
		}

		_DynamicUniformData._SpotLightAttenuationDistances[counter] = spotLightComponent->_AttenuationDistance;
		_DynamicUniformData._SpotLightIntensities[counter] = spotLightComponent->_Intensity;
		_DynamicUniformData._SpotLightInnerCutoffAngles[counter] = CatalystBaseMath::CosineDegrees(spotLightComponent->_InnerCutoffAngle);
		_DynamicUniformData._SpotLightOuterCutoffAngles[counter] = CatalystBaseMath::CosineDegrees(spotLightComponent->_OuterCutoffAngle);
		_DynamicUniformData._SpotLightColors[counter] = spotLightComponent->_Color;
		_DynamicUniformData._SpotLightDirections[counter] = Vector3(0.0f, -1.0f, 0.0f).Rotated(spotLightComponent->_Rotation);
		_DynamicUniformData._SpotLightDirections[counter]._Y *= -1.0f;
		_DynamicUniformData._SpotLightWorldPositions[counter] = spotLightComponent->_Position;

		++counter;
	}

	//Update the physics data.
	_DynamicUniformData._WindStrength = PhysicsSystem::Instance->GetWindStrength();
	_DynamicUniformData._WindDirection = PhysicsSystem::Instance->GetWindDirection();

	//Upload the dynamic uniform data to the uniform buffer.
	_FrameData.GetCurrentDynamicUniformDataBuffer()->UploadData(static_cast<void*>(&_DynamicUniformData));
}