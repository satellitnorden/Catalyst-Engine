#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/TextureData.h>
#include <Rendering/Engine/Viewer.h>
#include <Rendering/Engine/RenderPasses/RenderPasses.h>
#include <Rendering/ShaderData/Vulkan/VulkanShaderData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/LightingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanRenderingSystem);

/*
*	Pre-initializes the Vulkan rendering system.
*/
void VulkanRenderingSystem::PreInitializeSystem() NOEXCEPT
{
	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize();
}

/*
*	Post-initializes the Vulkan rendering system.
*/
void VulkanRenderingSystem::PostInitializeSystem() NOEXCEPT
{
	//Initialize all semaphores.
	InitializeSemaphores();

	//Initialize all shader modules.
	InitializeShaderModules();

	//Initialize all Vulkan render passes.
	InitializeVulkanRenderPasses();

	//Initialize the Vulkan frame data.
	_FrameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages());
}

/*
*	Pre-updates the Vulkan rendering system synchronously.
*/
void VulkanRenderingSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Pre-update the Vulkan interface.
	VulkanInterface::Instance->PreUpdate(_Semaphores[UNDERLYING(GraphicsSemaphore::ImageAvailable)]);

	//Process the destruction queue.
	ProcessDestructionQueue();

	//Begin the frame.
	BeginFrame();
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
	VulkanInterface::Instance->PostUpdate(_Semaphores[UNDERLYING(GraphicsSemaphore::RenderFinished)]);
}

/*
*	Releases the Vulkan rendering system.
*/
void VulkanRenderingSystem::ReleaseSystem() NOEXCEPT
{
	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Returns the number of frame buffers
*/
uint8 VulkanRenderingSystem::GetNumberOfFrameBuffers() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages();
}

/*
*	Returns the current frame buffer index.
*/
uint8 VulkanRenderingSystem::GetCurrentFrameBufferIndex() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();
}

/*
*	Creates a buffer.
*/
ConstantBufferHandle VulkanRenderingSystem::CreateBuffer(const uint64 size) const NOEXCEPT
{
	return reinterpret_cast<ConstantBufferHandle>(VulkanInterface::Instance->CreateBuffer(size));
}

/*
*	Uploads data to a buffer.
*/
void VulkanRenderingSystem::UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT dataSizes, const uint8 dataChunks, ConstantBufferHandle handle) const NOEXCEPT
{
	//Upload the data to the buffer.
	static_cast<VulkanConstantBuffer *const RESTRICT>(handle)->UploadData(data, static_cast<const VkDeviceSize *const RESTRICT>(dataSizes), dataChunks);
}

/*
*	Destroys a constant buffer.
*/
void VulkanRenderingSystem::DestroyConstantBuffer(ConstantBufferHandle handle) NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	_DestructionQueue.EmplaceSlow(VulkanDestructionData::Type::ConstantBuffer, handle);
}

/*
*	Creates a depth buffer.
*/
DepthBufferHandle VulkanRenderingSystem::CreateDepthBuffer(const Resolution resolution) const NOEXCEPT
{
	return static_cast<DepthBufferHandle>(VulkanInterface::Instance->CreateDepthBuffer(VulkanTranslationUtilities::GetVulkanExtent(resolution)));
}

/*
*	Destroys a render data table.
*/
void VulkanRenderingSystem::DestroyRenderDataTable(RenderDataTableHandle handle) NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	_DestructionQueue.EmplaceSlow(VulkanDestructionData::Type::RenderDataTable, handle);
}

/*
*	Creates a render target.
*/
void VulkanRenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render target.
	*handle = static_cast<RenderTargetHandle>(VulkanInterface::Instance->CreateRenderTarget(	VulkanTranslationUtilities::GetVulkanExtent(resolution),
																								VulkanTranslationUtilities::GetVulkanFormat(format)));
}

/*
*	Creates and returns a 2D texture given the texture data.
*/
Texture2DHandle VulkanRenderingSystem::Create2DTexture(const TextureData &textureData) const NOEXCEPT
{
	//Create the Vulkan 2D texture.
	Vulkan2DTexture *RESTRICT new2DTexture = VulkanInterface::Instance->Create2DTexture(static_cast<uint32>(textureData._TextureDataContainer._TextureData.Size()), textureData._TextureDataContainer._TextureWidth, textureData._TextureDataContainer._TextureHeight, textureData._TextureDataContainer._TextureChannels, textureData._TextureDataContainer._TextureTexelSize, textureData._TextureDataContainer._TextureData.Data(), VulkanTranslationUtilities::GetVulkanFormat(textureData._TextureFormat));

	//Return the texture.
	return static_cast<Texture2DHandle>(new2DTexture);
}

/*
*	Destroys a texture 2D
*/
void VulkanRenderingSystem::DestroyTexture2D(Texture2DHandle handle) NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	_DestructionQueue.EmplaceSlow(VulkanDestructionData::Type::Texture2D, handle);
}

/*
*	Creates a texture cube.
*/
TextureCubeHandle VulkanRenderingSystem::CreateTextureCube(const float *const RESTRICT data, const Resolution resolution) const NOEXCEPT
{
	return static_cast<TextureCubeHandle>(VulkanInterface::Instance->CreateCubeMapTexture(data, resolution._Width, resolution._Height));
}

/*
*	Creates and returns a sampler.
*/
SamplerHandle VulkanRenderingSystem::CreateSampler(const SamplerProperties &properties) const NOEXCEPT
{
	return VulkanInterface::Instance->CreateSampler(	VulkanTranslationUtilities::GetVulkanTextureFilter(properties._MagnificationFilter),
														VulkanTranslationUtilities::GetVulkanMipmapMode(properties._MipmapMode),
														VulkanTranslationUtilities::GetVulkanAddressMode(properties._AddressMode));
}

/*
*	Creates a uniform buffer and returns the identifier for the uniform buffer.
*/
UniformBufferHandle VulkanRenderingSystem::CreateUniformBuffer(const uint64 uniformBufferSize, const BufferUsage usage) const NOEXCEPT
{
	VkBufferUsageFlags vulkanUsage{ 0 };

	if (TEST_BIT(usage, BufferUsage::UniformBuffer))
	{
		vulkanUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}

	if (TEST_BIT(usage, BufferUsage::VertexBuffer))
	{
		vulkanUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}

	return VulkanInterface::Instance->CreateUniformBuffer(uniformBufferSize, vulkanUsage);
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
void VulkanRenderingSystem::DestroyUniformBuffer(UniformBufferHandle handle) NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	_DestructionQueue.EmplaceSlow(VulkanDestructionData::Type::UniformBuffer, handle);
}

/*
*	Finalizes the initialization of a render pass.
*/
void VulkanRenderingSystem::FinalizeRenderPassInitialization(RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Create the pipeline.
	VulkanPipelineCreationParameters parameters;

	parameters._BlendEnable = renderPass->IsBlendEnabled();
	parameters._BlendFactorSourceColor = VulkanTranslationUtilities::GetVulkanBlendFactor(renderPass->GetBlendFactorSourceColor());
	parameters._BlendFactorDestinationColor = VulkanTranslationUtilities::GetVulkanBlendFactor(renderPass->GetBlendFactorDestinationColor());
	parameters._BlendFactorSourceAlpha = VulkanTranslationUtilities::GetVulkanBlendFactor(renderPass->GetBlendFactorSourceAlpha());
	parameters._BlendFactorDestinationAlpha = VulkanTranslationUtilities::GetVulkanBlendFactor(renderPass->GetBlendFactorDestinationAlpha());
	parameters._ColorAttachmentCount = static_cast<uint32>(renderPass->GetRenderTargets().Size());
	parameters._CullMode = VulkanTranslationUtilities::GetVulkanCullMode(renderPass->GetCullMode());
	parameters._DepthCompareOp = VulkanTranslationUtilities::GetVulkanCompareOperator(renderPass->GetDepthCompareOperator());
	parameters._DepthTestEnable = renderPass->IsDepthTestEnabled();
	parameters._DepthWriteEnable = renderPass->IsDepthWriteEnabled();

	DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
	pipelineDescriptorSetLayouts.Reserve(renderPass->GetRenderDataTableLayouts().Size());

	for (RenderDataTableLayoutHandle renderDataTableLayout : renderPass->GetRenderDataTableLayouts())
	{
		pipelineDescriptorSetLayouts.EmplaceFast(*static_cast<VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
	}

	parameters._DescriptorSetLayoutCount = static_cast<uint32>(pipelineDescriptorSetLayouts.Size());
	parameters._DescriptorSetLayouts = pipelineDescriptorSetLayouts.Data();

	DynamicArray<VkPushConstantRange> pushConstantRanges;

	if (renderPass->GetPushConstantRanges().Empty())
	{
		parameters._PushConstantRangeCount = 0;
		parameters._PushConstantRanges = nullptr;
	}

	else
	{
		pushConstantRanges.Reserve(renderPass->GetPushConstantRanges().Size());

		for (const PushConstantRange &pushConstantRange : renderPass->GetPushConstantRanges())
		{
			pushConstantRanges.EmplaceFast(VulkanTranslationUtilities::GetVulkanPushConstantRange(pushConstantRange));
		}

		parameters._PushConstantRangeCount = static_cast<uint32>(pushConstantRanges.Size());
		parameters._PushConstantRanges = pushConstantRanges.Data();
	}
	
	parameters._ShaderModules.Reserve(5);
	if (renderPass->GetVertexShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[UNDERLYING(renderPass->GetVertexShader())]);
	if (renderPass->GetTessellationControlShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[UNDERLYING(renderPass->GetTessellationControlShader())]);
	if (renderPass->GetTessellationEvaluationShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[UNDERLYING(renderPass->GetTessellationEvaluationShader())]);
	if (renderPass->GetGeometryShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[UNDERLYING(renderPass->GetGeometryShader())]);
	if (renderPass->GetFragmentShader() != Shader::None) parameters._ShaderModules.EmplaceFast(_ShaderModules[UNDERLYING(renderPass->GetFragmentShader())]);
	parameters._StencilTestEnable = renderPass->IsStencilTestEnabled();
	parameters._StencilFailOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(renderPass->GetStencilFailOperator());
	parameters._StencilPassOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(renderPass->GetStencilPassOperator());
	parameters._StencilDepthFailOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(renderPass->GetStencilDepthFailOperator());
	parameters._StencilCompareOperator = VulkanTranslationUtilities::GetVulkanCompareOperator(renderPass->GetStencilCompareOperator());
	parameters._StencilCompareMask = renderPass->GetStencilCompareMask();
	parameters._StencilWriteMask = renderPass->GetStencilWriteMask();
	parameters._StencilReferenceMask = renderPass->GetStencilReferenceMask();
	parameters._Subpass = VulkanTranslationUtilities::GetSubStageIndex(renderPass->GetMainStage(), renderPass->GetSubStage());

	parameters._Topology = VulkanTranslationUtilities::GetVulkanTopology(renderPass->GetTopology());

	DynamicArray<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	vertexInputAttributeDescriptions.Reserve(renderPass->GetVertexInputAttributeDescriptions().Size());

	for (const VertexInputAttributeDescription &vertexInputAttributeDescription : renderPass->GetVertexInputAttributeDescriptions())
	{
		vertexInputAttributeDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputAttributeDescription(vertexInputAttributeDescription));
	}

	parameters._VertexInputAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.Size());
	parameters._VertexInputAttributeDescriptions = vertexInputAttributeDescriptions.Data();

	DynamicArray<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	vertexInputBindingDescriptions.Reserve(renderPass->GetVertexInputBindingDescriptions().Size());

	for (const VertexInputBindingDescription &vertexInputBindingDescription : renderPass->GetVertexInputBindingDescriptions())
	{
		vertexInputBindingDescriptions.EmplaceFast(VulkanTranslationUtilities::GetVulkanVertexInputBindingDescription(vertexInputBindingDescription));
	}

	parameters._VertexInputBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.Size());
	parameters._VertexInputBindingDescriptions = vertexInputBindingDescriptions.Data();
	parameters._ViewportExtent = renderPass->GetRenderTargets().Empty() ? VkExtent2D{ renderPass->GetRenderResolution()._Width, renderPass->GetRenderResolution()._Height } : renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution()._Width, renderPass->GetRenderResolution()._Height };

	parameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(renderPass->GetMainStage())]._RenderPass;

	//Create the pipeline!
	_VulkanRenderPassSubStageData[UNDERLYING(renderPass->GetSubStage())]._Pipeline = VulkanInterface::Instance->CreatePipeline(parameters);

	//Update the Vulkan render pass data.
	_VulkanRenderPassSubStageData[UNDERLYING(renderPass->GetSubStage())]._Framebuffers.Reserve(_VulkanRenderPassMainStageData[UNDERLYING(renderPass->GetMainStage())]._FrameBuffers.Size());

	for (VulkanFramebuffer *RESTRICT vulkanFrameBuffer : _VulkanRenderPassMainStageData[UNDERLYING(renderPass->GetMainStage())]._FrameBuffers)
	{
		_VulkanRenderPassSubStageData[UNDERLYING(renderPass->GetSubStage())]._Framebuffers.EmplaceFast(vulkanFrameBuffer->Get());
	}

	_VulkanRenderPassSubStageData[UNDERLYING(renderPass->GetSubStage())]._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(renderPass->GetMainStage())]._RenderPass->Get();

	renderPass->SetData(&_VulkanRenderPassSubStageData[UNDERLYING(renderPass->GetSubStage())]);

	//Add the command buffers.
	const uint64 numberOfCommandBuffers{ VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() };
	renderPass->SetNumberOfCommandBuffers(numberOfCommandBuffers);

	//Create the directional shadow command pool.
	VulkanCommandPool *const RESTRICT pipelineCommandPool{ VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) };

	for (uint64 i = 0; i < numberOfCommandBuffers; ++i)
	{
		VulkanCommandBuffer *const RESTRICT pipelineCommandBuffer{ new (MemoryUtilities::GlobalPoolAllocate<sizeof(VulkanCommandBuffer)>()) VulkanCommandBuffer };
		pipelineCommandPool->AllocateSecondaryCommandBuffer(*pipelineCommandBuffer);
		renderPass->AddCommandBuffer(new (MemoryUtilities::GlobalPoolAllocate<sizeof(CommandBuffer)>()) CommandBuffer(pipelineCommandBuffer));
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

		vulkanBindings.EmplaceFast(VulkanUtilities::CreateDescriptorSetLayoutBinding(binding._Binding, VulkanTranslationUtilities::GetVulkanDescriptorType(binding._Type), binding._NumberOfArrayElements, VulkanTranslationUtilities::GetVulkanShaderStages(binding._ShaderStage)));
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
*	Binds a combined image sampler to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void VulkanRenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(renderDataTable) };
	VulkanImage *const RESTRICT vulkanImage{ static_cast<VulkanImage *const RESTRICT>(image) };
	VulkanSampler *const RESTRICT vulkanSampler{ static_cast<VulkanSampler *const RESTRICT>(sampler) };

	//Create the destriptor image info.
	VkDescriptorImageInfo descriptorImageInfo;

	descriptorImageInfo.sampler = vulkanSampler->Get();
	descriptorImageInfo.imageView = vulkanImage->GetImageView();
	descriptorImageInfo.imageLayout = vulkanImage->GetImageLayout();

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a sampled image to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void VulkanRenderingSystem::BindSampledImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, OpaqueHandle image) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(renderDataTable) };
	VulkanImage *const RESTRICT vulkanImage{ static_cast<VulkanImage *const RESTRICT>(image) };

	//Create the destriptor image info.
	VkDescriptorImageInfo descriptorImageInfo;

	descriptorImageInfo.sampler = VK_NULL_HANDLE;
	descriptorImageInfo.imageView = vulkanImage->GetImageView();
	descriptorImageInfo.imageLayout = vulkanImage->GetImageLayout();

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a sampler to a render data table.
*/
void VulkanRenderingSystem::BindSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, SamplerHandle sampler) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(renderDataTable) };
	VulkanSampler *const RESTRICT vulkanSampler{ static_cast<VulkanSampler *const RESTRICT>(sampler) };

	//Create the destriptor image info.
	VkDescriptorImageInfo descriptorImageInfo;

	descriptorImageInfo.sampler = vulkanSampler->Get();
	descriptorImageInfo.imageView = VK_NULL_HANDLE;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a uniform buffer to a render data table.
*/
void VulkanRenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle renderDataTable, UniformBufferHandle uniformBuffer) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(renderDataTable) };
	VulkanUniformBuffer *const RESTRICT vulkanUniformBuffer{ static_cast<VulkanUniformBuffer *const RESTRICT>(uniformBuffer) };

	//Create the destriptor buffer info.
	VkDescriptorBufferInfo descriptorBufferInfo;

	descriptorBufferInfo.buffer = vulkanUniformBuffer->Get();
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = vulkanUniformBuffer->GetSize();

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pImageInfo = nullptr;
	writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Initializes all semaphores.
*/
void VulkanRenderingSystem::InitializeSemaphores() NOEXCEPT
{
	//Initialize all semaphores.
	_Semaphores[UNDERLYING(GraphicsSemaphore::ImageAvailable)] = VulkanInterface::Instance->CreateSemaphore();
	_Semaphores[UNDERLYING(GraphicsSemaphore::RenderFinished)] = VulkanInterface::Instance->CreateSemaphore();
}

/*
*	Initializes all shader modules.
*/
void VulkanRenderingSystem::InitializeShaderModules() NOEXCEPT
{
	{
		//Initialize the above ocean fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetAboveOceanFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::AboveOceanFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the anti-aliasing fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetAntiAliasingFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::AntiAliasingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the below ocean fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBelowOceanFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::BelowOceanFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the bloom horizontal fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBloomHorizontalFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::BloomHorizontalFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the bloom vertical fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBloomVerticalFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::BloomVerticalFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the box blur shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBoxBlurFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::BoxBlurFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

#if !defined(CATALYST_FINAL)
	{
		//Initialize the debug axis-aligned bounding box fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDebugAxisAlignedBoundingBoxFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DebugAxisAlignedBoundingBoxFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the debug axis-aligned bounding box vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDebugAxisAlignedBoundingBoxVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DebugAxisAlignedBoundingBoxVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the debug screen box fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDebugScreenBoxFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DebugScreenBoxFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the debug screen box vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDebugScreenBoxVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DebugScreenBoxVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
#endif

	{
		//Initialize the depth of field fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDepthOfFieldFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DepthOfFieldFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the directional physical shadow vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalPhysicalShadowVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DirectionalPhysicalShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the directional shadow fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DirectionalShadowFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize directional solid vegetation shadow fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalSolidVegetationShadowFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DirectionalSolidVegetationShadowFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize directional solid vegetation shadow fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalSolidVegetationShadowVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DirectionalSolidVegetationShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalTerrainShadowVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DirectionalTerrainShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional tree vegetation trunk vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalTreeVegetationTrunkVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DirectionalTreeVegetationTrunkVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetGaussianBlurFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::GaussianBlurFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail debris vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailDebrisVegetationFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailDebrisVegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail debris vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailDebrisVegetationVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailDebrisVegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the grass vegetation color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailGrassVegetationColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailGrassVegetationColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the grass vegetation color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailGrassVegetationColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailGrassVegetationColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the grass vegetation depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailGrassVegetationDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailGrassVegetationDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the grass vegetation depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailGrassVegetationDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailGrassVegetationDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the high detail solid vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailSolidVegetationFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailSolidVegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail solid vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailSolidVegetationVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailSolidVegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the high detail tree vegetation crown color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationCrownColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationCrownColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail tree vegetation crown color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationCrownColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationCrownColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the high detail tree vegetation crown depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationCrownDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationCrownDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail tree vegetation crown depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationCrownDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationCrownDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the high detail tree vegetation impostor color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationImpostorColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationImpostorColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail tree vegetation impostor color geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationImpostorColorGeometryShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationImpostorColorGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the high detail tree vegetation impostor color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationImpostorColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationImpostorColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the high detail tree vegetation impostor depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationImpostorDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationImpostorDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail tree vegetation impostor depth geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationImpostorDepthGeometryShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationImpostorDepthGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the high detail tree vegetation impostor depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationImpostorDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationImpostorDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the high detail tree vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationTrunkFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationTrunkFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail tree vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTreeVegetationTrunkVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTreeVegetationTrunkVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the lighting fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLightingFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LightingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail debris vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailDebrisVegetationFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailDebrisVegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail debris vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailDebrisVegetationVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailDebrisVegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the low detail grass vegetation color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailGrassVegetationColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailGrassVegetationColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail grass vegetation color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailGrassVegetationColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailGrassVegetationColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the low detail grass vegetation depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailGrassVegetationDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailGrassVegetationDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail grass vegetation depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailGrassVegetationDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailGrassVegetationDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the low detail solid vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailSolidVegetationFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailSolidVegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail solid vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailSolidVegetationVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailSolidVegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the low detail tree vegetation crown color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationCrownColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationCrownColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail tree vegetation crown color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationCrownColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationCrownColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the low detail tree vegetation crown depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationCrownDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationCrownDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail tree vegetation crown depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationCrownDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationCrownDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the low detail tree vegetation impostor color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationImpostorColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationImpostorColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail tree vegetation impostor color geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationImpostorColorGeometryShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationImpostorColorGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the low detail tree vegetation impostor color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationImpostorColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationImpostorColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the low detail tree vegetation impostor depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationImpostorDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationImpostorDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail tree vegetation impostor depth geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationImpostorDepthGeometryShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationImpostorDepthGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the low detail tree vegetation impostor depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationImpostorDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationImpostorDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the low detail tree vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationTrunkFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationTrunkFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the low detail tree vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLowDetailTreeVegetationTrunkVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LowDetailTreeVegetationTrunkVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the medium detail debris vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailDebrisVegetationFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailDebrisVegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the medium detail debris vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailDebrisVegetationVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailDebrisVegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the medium detail grass vegetation color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailGrassVegetationColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailGrassVegetationColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the medium detail grass vegetation color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailGrassVegetationColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailGrassVegetationColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the medium detail grass vegetation depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailGrassVegetationDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailGrassVegetationDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the medium detail grass vegetation depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailGrassVegetationDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailGrassVegetationDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the medium detail solid vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailSolidVegetationFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailSolidVegetationFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the medium detail solid vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailSolidVegetationVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailSolidVegetationVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the medium detail tree vegetation crown color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailTreeVegetationCrownColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailTreeVegetationCrownColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the medium detail tree vegetation crown color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailTreeVegetationCrownColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailTreeVegetationCrownColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the medium detail tree vegetation crown depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailTreeVegetationCrownDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailTreeVegetationCrownDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the medium detail tree vegetation crown depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailTreeVegetationCrownDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailTreeVegetationCrownDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the medium detail tree vegetation fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailTreeVegetationTrunkFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailTreeVegetationTrunkFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the medium detail tree vegetation vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetMediumDetailTreeVegetationTrunkVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::MediumDetailTreeVegetationTrunkVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the outline fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetOutlineFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::OutlineFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the particle system fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::ParticleSystemFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the particle system geometry shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemGeometryShaderData(data);
		_ShaderModules[UNDERLYING(Shader::ParticleSystemGeometry)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_GEOMETRY_BIT);
	}

	{
		//Initialize the particle system vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetParticleSystemVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::ParticleSystemVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the physical fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPhysicalFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::PhysicalFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	
	{
		//Initialize the physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPhysicalVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::PhysicalVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}
	
	{
		//Initialize the screen space ambient occlusion fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetScreenSpaceAmbientOcclusionFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::ScreenSpaceAmbientOcclusionFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the shadow map fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetShadowMapFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::ShadowMapFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the sky fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetSkyFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::SkyFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the terrain color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::TerrainColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the terrain depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::TerrainDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the terrain vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::TerrainVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the tone mapping fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetToneMappingFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::ToneMappingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the viewport vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetViewportVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::ViewportVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the volumetric fog fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVolumetricFogFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::VolumetricFogFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
}

/*
*	Initializes all Vulkan render passes.
*/
void VulkanRenderingSystem::InitializeVulkanRenderPasses() NOEXCEPT
{
	//Initialize the directional shadow mapping render pass.
	{
		constexpr uint64 NUMBER_OF_DIRECTIONAL_SHADOW_SUBPASSES{ 4 };

		constexpr uint32 DEPTH_BUFFER_INDEX{ 0 };
		constexpr uint32 SHADOW_MAP_INDEX{ 1 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 2> attachmenDescriptions
		{
			//Depth buffer.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::DirectionalLight))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),

			//Shadow map.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadowMap))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SHADOW_MAP_INDEX, VK_IMAGE_LAYOUT_GENERAL }
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
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	2,
														3,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT)
		};

		renderPassParameters._SubpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters._SubpassDependencies = subpassDependencies.Data();

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowMapping)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowMapping)]._RenderPass->Get();
		
		StaticArray<VkImageView, 2> attachments
		{
			static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::DirectionalLight))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadowMap))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowMapping)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowMapping)]._FrameBuffers.EmplaceFast( VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowMapping)]._NumberOfAttachments = 2;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowMapping)]._ShouldClear = true;
	}

	//Initialize the scene render pass.
	{
		constexpr uint64 NUMBER_OF_SUBPASSES{ 28 };

		constexpr uint32 DEPTH_BUFFER_INDEX{ 0 };
		constexpr uint32 ALBEDO_INDEX{ 1 };
		constexpr uint32 NORMAL_DEPTH_INDEX{ 2 };
		constexpr uint32 MATERIAL_PROPERTIES_INDEX{ 3 };
		constexpr uint32 SCENE_INTERMEDIATE_INDEX{ 4 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 5> attachmenDescriptions
		{
			//Depth buffer.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),
			
			//Albedo.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferAlbedo))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL),

			//Normal depth.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL),

			//Material properties.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferMaterialProperties))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL),

			//Scene intermediate.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 3> sceneBufferColorAttachmentReferences
		{
			VkAttachmentReference{ ALBEDO_INDEX, VK_IMAGE_LAYOUT_GENERAL },
			VkAttachmentReference{ NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_GENERAL },
			VkAttachmentReference{ MATERIAL_PROPERTIES_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		constexpr VkAttachmentReference terrainDepthAttachmentReference{ SCENE_INTERMEDIATE_INDEX, VK_IMAGE_LAYOUT_GENERAL };
		constexpr VkAttachmentReference normalDepthAttachmentReference{  NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_GENERAL };
		constexpr VkAttachmentReference depthAttachmentReference{ DEPTH_BUFFER_INDEX, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		constexpr VkAttachmentReference normalDepthInputAttachmentReference{ VulkanUtilities::CreateAttachmentReference(NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_GENERAL) };

		StaticArray<VkSubpassDescription, NUMBER_OF_SUBPASSES> subpassDescriptions;

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::TerrainDepth)]
			= VulkanUtilities::CreateSubpassDescription(	0,
															nullptr,
															1,
															&terrainDepthAttachmentReference,
															&depthAttachmentReference,
															0,
															nullptr);

		constexpr VkAttachmentReference terrainColorInputAttachmentReference{ VulkanUtilities::CreateAttachmentReference(NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_GENERAL) };

		constexpr StaticArray<const VkAttachmentReference, 3> highDetailTerrainColorAttachmentReferences
		{
			VkAttachmentReference{ ALBEDO_INDEX, VK_IMAGE_LAYOUT_GENERAL },
			VkAttachmentReference{ NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_GENERAL },
			VkAttachmentReference{ MATERIAL_PROPERTIES_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		constexpr StaticArray<const VkAttachmentReference, 2> lowDetailTerrainColorAttachmentReferences
		{
			VkAttachmentReference{ ALBEDO_INDEX, VK_IMAGE_LAYOUT_GENERAL },
			VkAttachmentReference{ MATERIAL_PROPERTIES_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::TerrainColor)] =
								VulkanUtilities::CreateSubpassDescription(	1,
																			&terrainColorInputAttachmentReference,
																			static_cast<uint32>(highDetailTerrainColorAttachmentReferences.Size()),
																			highDetailTerrainColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::DynamicPhysical)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationTrunk)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationTrunk)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationTrunk)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationCrownDepth)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			0,
																			nullptr,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationCrownColor)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationCrownDepth)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			0,
																			nullptr,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationCrownColor)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationCrownDepth)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			0,
																			nullptr,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationCrownColor)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationImpostorDepth)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			0,
																			nullptr,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationImpostorColor)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationImpostorDepth)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			0,
																			nullptr,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationImpostorColor)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailSolidVegetation)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailSolidVegetation)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailSolidVegetation)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailDebrisVegetation)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailDebrisVegetation)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailDebrisVegetation)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailGrassVegetationDepth)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			0,
																			nullptr,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::HighDetailGrassVegetationColor)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailGrassVegetationDepth)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			0,
																			nullptr,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::LowDetailGrassVegetationColor)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailGrassVegetationDepth)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			0,
																			nullptr,
																			&depthAttachmentReference,
																			0,
																			nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(	RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailGrassVegetationColor)] =
								VulkanUtilities::CreateSubpassDescription(	0,
																			nullptr,
																			static_cast<uint32>(sceneBufferColorAttachmentReferences.Size()),
																			sceneBufferColorAttachmentReferences.Data(),
																			&depthAttachmentReference,
																			0,
																			nullptr);

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		StaticArray<VkSubpassDependency, NUMBER_OF_SUBPASSES - 1> subpassDependencies
		{
			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::TerrainDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::TerrainColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::TerrainColor),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::DynamicPhysical),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::DynamicPhysical),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationTrunk),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationTrunk),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationTrunk),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationTrunk),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationTrunk),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationTrunk),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationCrownDepth),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationCrownDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationCrownColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationCrownColor),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationCrownDepth),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationCrownDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationCrownColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailTreeVegetationCrownColor),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationCrownDepth),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationCrownDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationCrownColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationCrownColor),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationImpostorDepth),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationImpostorDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationImpostorColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailTreeVegetationImpostorColor),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationImpostorDepth),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationImpostorDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationImpostorColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailTreeVegetationImpostorColor),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailSolidVegetation),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailSolidVegetation),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailSolidVegetation),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailSolidVegetation),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailSolidVegetation),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailSolidVegetation),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailDebrisVegetation),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailDebrisVegetation),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailDebrisVegetation),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailDebrisVegetation),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailDebrisVegetation),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailDebrisVegetation),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailGrassVegetationDepth),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailGrassVegetationDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailGrassVegetationColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::HighDetailGrassVegetationColor),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailGrassVegetationDepth),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailGrassVegetationDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailGrassVegetationColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::MediumDetailGrassVegetationColor),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailGrassVegetationDepth),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(	VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailGrassVegetationDepth),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Scene, RenderPassSubStage::LowDetailGrassVegetationColor),
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT)
		};

		renderPassParameters._SubpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters._SubpassDependencies = subpassDependencies.Data();

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._RenderPass->Get();

		StaticArray<VkImageView, 5> attachments
		{
			static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferAlbedo))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferMaterialProperties))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._NumberOfAttachments = static_cast<uint32>(attachments.Size());
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._ShouldClear = true;
	}

	//Initialize the directional shadow calculation render pass.
	{
		constexpr uint64 NUMBER_OF_SUBPASSES{ 1 };

		constexpr uint32 DIRECTIONAL_SHADOW_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Directional shadow.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadow))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr VkAttachmentReference colorAttachmentReference{ DIRECTIONAL_SHADOW_INDEX, VK_IMAGE_LAYOUT_GENERAL };

		StaticArray<VkSubpassDescription, NUMBER_OF_SUBPASSES> subpassDescriptions;

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::DirectionalShadowCalculation, RenderPassSubStage::DirectionalShadow)]
			= VulkanUtilities::CreateSubpassDescription(	0,
															nullptr,
															1,
															&colorAttachmentReference,
															nullptr,
															0,
															nullptr);

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		renderPassParameters._SubpassDependencyCount = 0;
		renderPassParameters._SubpassDependencies = nullptr;

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowCalculation)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowCalculation)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadow))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowCalculation)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowCalculation)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowCalculation)]._NumberOfAttachments = static_cast<uint32>(attachments.Size());
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowCalculation)]._ShouldClear = false;
	}

	//Initialize the directional shadow horizontal blur render pass.
	{
		constexpr uint64 NUMBER_OF_SUBPASSES{ 1 };

		constexpr uint32 SCENE_INTERMEDIATE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Directional shadow.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr VkAttachmentReference colorAttachmentReference{ SCENE_INTERMEDIATE_INDEX, VK_IMAGE_LAYOUT_GENERAL };

		StaticArray<VkSubpassDescription, NUMBER_OF_SUBPASSES> subpassDescriptions;

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::DirectionalShadowHorizontalBlur, RenderPassSubStage::DirectionalShadowHorizontalBlur)]
			= VulkanUtilities::CreateSubpassDescription(	0,
															nullptr,
															1,
															&colorAttachmentReference,
															nullptr,
															0,
															nullptr);

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		renderPassParameters._SubpassDependencyCount = 0;
		renderPassParameters._SubpassDependencies = nullptr;

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowHorizontalBlur)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowHorizontalBlur)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowHorizontalBlur)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowHorizontalBlur)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowHorizontalBlur)]._NumberOfAttachments = static_cast<uint32>(attachments.Size());
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowHorizontalBlur)]._ShouldClear = false;
	}

	//Initialize the directional shadow horizontal blur render pass.
	{
		constexpr uint64 NUMBER_OF_SUBPASSES{ 1 };

		constexpr uint32 DIRECTIONAL_SHADOW_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Directional shadow.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadow))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr VkAttachmentReference colorAttachmentReference{ DIRECTIONAL_SHADOW_INDEX, VK_IMAGE_LAYOUT_GENERAL };

		StaticArray<VkSubpassDescription, NUMBER_OF_SUBPASSES> subpassDescriptions;

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::DirectionalShadowVerticalBlur, RenderPassSubStage::DirectionalShadowVerticalBlur)]
			= VulkanUtilities::CreateSubpassDescription(	0,
															nullptr,
															1,
															&colorAttachmentReference,
															nullptr,
															0,
															nullptr);

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		renderPassParameters._SubpassDependencyCount = 0;
		renderPassParameters._SubpassDependencies = nullptr;

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowVerticalBlur)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowVerticalBlur)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadow))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowVerticalBlur)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowVerticalBlur)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowVerticalBlur)]._NumberOfAttachments = static_cast<uint32>(attachments.Size());
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadowVerticalBlur)]._ShouldClear = false;
	}

	//Initialize the screen space ambient occlusion render pass.
	{
		constexpr uint64 SUBPASSES{ 1 };

		constexpr uint32 SCREEN_SPACE_AMBIENT_OCCLUSION_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Screen.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::ScreenSpaceAmbientOcclusion))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCREEN_SPACE_AMBIENT_OCCLUSION_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		StaticArray<VkSubpassDescription, SUBPASSES> subpassDescriptions;

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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ScreenSpaceAmbientOcclusionCalculation)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ScreenSpaceAmbientOcclusionCalculation)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::ScreenSpaceAmbientOcclusion))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ScreenSpaceAmbientOcclusionCalculation)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ScreenSpaceAmbientOcclusionCalculation)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ScreenSpaceAmbientOcclusionCalculation)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ScreenSpaceAmbientOcclusionCalculation)]._ShouldClear = false;
	}

	//Initialize the lighting render pass.
	{
		constexpr uint64 NUMBER_OF_SUBPASSES{ 3 };

		constexpr uint32 DEPTH_BUFFER_INDEX{ 0 };
		constexpr uint32 SCENE_INDEX{ 1 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 2> attachmenDescriptions
		{
			//Depth buffer.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),

			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		StaticArray<VkSubpassDescription, NUMBER_OF_SUBPASSES> subpassDescriptions;

		constexpr VkAttachmentReference depthAttachmentReference{ DEPTH_BUFFER_INDEX, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		constexpr VkAttachmentReference sceneColorAttachmentReference{ VulkanUtilities::CreateAttachmentReference(SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL) };

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Lighting, RenderPassSubStage::Lighting)] =
			VulkanUtilities::CreateSubpassDescription(0,
				nullptr,
				1,
				&sceneColorAttachmentReference,
				&depthAttachmentReference,
				0,
				nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Lighting, RenderPassSubStage::Sky)] =
			VulkanUtilities::CreateSubpassDescription(0,
				nullptr,
				1,
				&sceneColorAttachmentReference,
				&depthAttachmentReference,
				0,
				nullptr);

		subpassDescriptions[VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Lighting, RenderPassSubStage::DynamicOutline)] =
			VulkanUtilities::CreateSubpassDescription(0,
				nullptr,
				1,
				&sceneColorAttachmentReference,
				&depthAttachmentReference,
				0,
				nullptr);

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		StaticArray<VkSubpassDependency, NUMBER_OF_SUBPASSES - 1> subpassDependencies
		{
			VulkanUtilities::CreateSubpassDependency(VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Lighting, RenderPassSubStage::Lighting),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Lighting, RenderPassSubStage::Sky),
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),

			VulkanUtilities::CreateSubpassDependency(VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Lighting, RenderPassSubStage::Sky),
														VulkanTranslationUtilities::GetSubStageIndex(RenderPassMainStage::Lighting, RenderPassSubStage::DynamicOutline),
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_DEPENDENCY_BY_REGION_BIT)
		};

		renderPassParameters._SubpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters._SubpassDependencies = subpassDependencies.Data();

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Lighting)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Lighting)]._RenderPass->Get();

		StaticArray<VkImageView, 2> attachments
		{
			static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Lighting)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Lighting)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Lighting)]._NumberOfAttachments = static_cast<uint32>(attachments.Size());
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Lighting)]._ShouldClear = false;
	}

	//Initialize the particle system render pass.
	{
		constexpr uint64 NUMBER_OF_SUBPASSES{ 1 };

		constexpr uint32 DEPTH_BUFFER_INDEX{ 0 };
		constexpr uint32 SCENE_INDEX{ 1 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 2> attachmenDescriptions
		{
			//Depth buffer.
			VulkanUtilities::CreateAttachmentDescription(static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),

			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr VkAttachmentReference depthAttachmentReference{ DEPTH_BUFFER_INDEX, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		
		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL },
		};

		VkSubpassDescription subpassDescription
		{
			VulkanUtilities::CreateSubpassDescription(	0,
														nullptr,
														static_cast<uint32>(colorAttachmentReferences.Size()),
														colorAttachmentReferences.Data(),
														&depthAttachmentReference,
														0,
														nullptr)
		};

		renderPassParameters._SubpassDescriptionCount = 1;
		renderPassParameters._SubpassDescriptions = &subpassDescription;

		renderPassParameters._SubpassDependencyCount = 0;
		renderPassParameters._SubpassDependencies = nullptr;

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ParticleSystem)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ParticleSystem)]._RenderPass->Get();

		StaticArray<VkImageView, 2> attachments
		{
			static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ParticleSystem)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ParticleSystem)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ParticleSystem)]._NumberOfAttachments = static_cast<uint32>(attachments.Size());
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ParticleSystem)]._ShouldClear = false;
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
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),

			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr VkAttachmentReference depthAttachmentReference{ DEPTH_BUFFER_INDEX, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._RenderPass->Get();

		StaticArray<VkImageView, 2> attachments
		{
			static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Debug)]._ShouldClear = false;
	}
#endif

	//Initialize the ocean render pass.
	{
		constexpr uint64 NUMBER_OF_OCEAN_SUBPASSES{ 2 };

		constexpr uint32 SCENE_INDEX{ 0 };
		constexpr uint32 NORMAL_DEPTH_INDEX{ 1 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 2> attachmenDescriptions
		{
			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL),

			//Normal/depth.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 2> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL },
			VkAttachmentReference{ NORMAL_DEPTH_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		StaticArray<VkSubpassDescription, NUMBER_OF_OCEAN_SUBPASSES> subpassDescriptions
		{
			VulkanUtilities::CreateSubpassDescription(	0,
														nullptr,
														static_cast<uint32>(colorAttachmentReferences.Size()),
														colorAttachmentReferences.Data(),
														nullptr,
														0,
														nullptr),

			VulkanUtilities::CreateSubpassDescription(	0,
														nullptr,
														static_cast<uint32>(colorAttachmentReferences.Size()),
														colorAttachmentReferences.Data(),
														nullptr,
														0,
														nullptr)
		};

		renderPassParameters._SubpassDescriptionCount = static_cast<uint32>(subpassDescriptions.Size());
		renderPassParameters._SubpassDescriptions = subpassDescriptions.Data();

		StaticArray<VkSubpassDependency, NUMBER_OF_OCEAN_SUBPASSES - 1> subpassDependencies
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
		renderPassParameters._SubpassDependencies = subpassDependencies.Data();

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._RenderPass->Get();

		StaticArray<VkImageView, 2> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._NumberOfAttachments = static_cast<uint32>(attachments.Size());
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._ShouldClear = false;
	}

	//Initialize the volumetric fog render pass.
	{
		constexpr uint64 NUMBER_OF_VOLUMETRIC_FOG_SUBPASSES{ 1 };

		constexpr uint32 SCENE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_LOAD,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		VkSubpassDescription subpassDescription
		{
			VulkanUtilities::CreateSubpassDescription(	0,
														nullptr,
														1,
														colorAttachmentReferences.Data(),
														nullptr,
														0,
														nullptr)
		};

		renderPassParameters._SubpassDescriptionCount = 1;
		renderPassParameters._SubpassDescriptions = &subpassDescription;

		renderPassParameters._SubpassDependencyCount = 0;
		renderPassParameters._SubpassDependencies = nullptr;

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::VolumetricFog)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::VolumetricFog)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::VolumetricFog)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::VolumetricFog)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::VolumetricFog)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::VolumetricFog)]._ShouldClear = false;
	}

	//Initialize the bloom horizontal render pass.
	{
		constexpr uint64 SUBPASSES{ 1 };

		constexpr uint32 SCENE_INTERMEDIATE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Screen.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INTERMEDIATE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		StaticArray<VkSubpassDescription, SUBPASSES> subpassDescriptions;

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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomHorizontal)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomHorizontal)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomHorizontal)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomHorizontal)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomHorizontal)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomHorizontal)]._ShouldClear = false;
	}

	//Initialize the bloom vertical render pass.
	{
		constexpr uint64 SUBPASSES{ 1 };

		constexpr uint32 SCENE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Screen.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		StaticArray<VkSubpassDescription, SUBPASSES> subpassDescriptions;

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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomVertical)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomVertical)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomVertical)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomVertical)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomVertical)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::BloomVertical)]._ShouldClear = false;
	}

	//Initialize the depth of field horizontal render pass.
	{
		constexpr uint64 SUBPASSES{ 1 };

		constexpr uint32 SCENE_INTERMEDIATE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Screen.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INTERMEDIATE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		StaticArray<VkSubpassDescription, SUBPASSES> subpassDescriptions;

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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldHorizontal)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldHorizontal)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldHorizontal)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldHorizontal)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldHorizontal)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldHorizontal)]._ShouldClear = false;
	}

	//Initialize the depth of field vertical render pass.
	{
		constexpr uint64 SUBPASSES{ 1 };

		constexpr uint32 SCENE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Screen.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		StaticArray<VkSubpassDescription, SUBPASSES> subpassDescriptions;

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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldVertical)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldVertical)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldVertical)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldVertical)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldVertical)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DepthOfFieldVertical)]._ShouldClear = false;
	}

	//Initialize the tone mapping render pass.
	{
		constexpr uint64 NUMBER_OF_TONE_MAPPING_SUBPASSES{ 1 };

		constexpr uint32 SCENE_INTERMEDIATE_INDEX{ 0 };

		VulkanRenderPassCreationParameters renderPassParameters;

		StaticArray<VkAttachmentDescription, 1> attachmenDescriptions
		{
			//Screen.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_GENERAL,
															VK_IMAGE_LAYOUT_GENERAL)
		};

		renderPassParameters._AttachmentCount = static_cast<uint32>(attachmenDescriptions.Size());
		renderPassParameters._AttachmentDescriptions = attachmenDescriptions.Data();

		constexpr StaticArray<const VkAttachmentReference, 1> colorAttachmentReferences
		{
			VkAttachmentReference{ SCENE_INTERMEDIATE_INDEX, VK_IMAGE_LAYOUT_GENERAL }
		};

		StaticArray<VkSubpassDescription, NUMBER_OF_TONE_MAPPING_SUBPASSES> subpassDescriptions;

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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._RenderPass->Get();

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::ToneMapping)]._ShouldClear = false;
	}

	//Initialize the anti-aliasing final render pass.
	{
		constexpr uint64 NUMBER_OF_ANTI_ALIASING_SUBPASSES{ 1 };

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

		StaticArray<VkSubpassDescription, NUMBER_OF_ANTI_ALIASING_SUBPASSES> subpassDescriptions;

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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffers.
		const DynamicArray<VkImageView> &swapchainImages{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._FrameBuffers.Reserve(swapchainImages.Size());

		for (VkImageView swapchainImage : swapchainImages)
		{
			VulkanFramebufferCreationParameters framebufferParameters;

			framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._RenderPass->Get();

			framebufferParameters._AttachmentCount = 1;
			framebufferParameters._Attachments = &swapchainImage;
			framebufferParameters._Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

			_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		}

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::AntiAliasing)]._ShouldClear = false;
	}
}

/*
*	Processes the destruction queue.
*/
void VulkanRenderingSystem::ProcessDestructionQueue() NOEXCEPT
{
	for (uint64 i = 0; i < _DestructionQueue.Size();)
	{
		++_DestructionQueue[i]._Frames;

		if (_DestructionQueue[i]._Frames > VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() + 1)
		{
			switch (_DestructionQueue[i]._Type)
			{
				case VulkanDestructionData::Type::ConstantBuffer:
				{
					VulkanInterface::Instance->DestroyConstantBuffer(static_cast<VulkanConstantBuffer *const RESTRICT>(_DestructionQueue[i]._Handle));

					break;
				}

				case VulkanDestructionData::Type::RenderDataTable:
				{
					VulkanInterface::Instance->DestroyDescriptorSet(static_cast<VulkanDescriptorSet *const RESTRICT>(_DestructionQueue[i]._Handle));

					break;
				}

				case VulkanDestructionData::Type::Texture2D:
				{
					VulkanInterface::Instance->Destroy2DTexture(static_cast<Vulkan2DTexture *const RESTRICT>(_DestructionQueue[i]._Handle));

					break;
				}

				case VulkanDestructionData::Type::UniformBuffer:
				{
					VulkanInterface::Instance->DestroyUniformBuffer(static_cast<VulkanUniformBuffer *const RESTRICT>(_DestructionQueue[i]._Handle));

					break;
				}
#if !defined(CATALYST_FINAL)
				default:
				{
					ASSERT(false, "Unhandled case!");

					break;
				}
#endif
			}

			_DestructionQueue.EraseAt(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Begins the frame.
*/
void VulkanRenderingSystem::BeginFrame() NOEXCEPT
{
	//Set the current frame.
	_FrameData.SetCurrentFrame(VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());

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

			//Specialization - Copy scene to scene intermediate for the ocean render pass.
			if (currentStage == RenderPassMainStage::Ocean)
			{
				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
																		VK_ACCESS_TRANSFER_READ_BIT,
																		VK_IMAGE_LAYOUT_GENERAL,
																		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
																		static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImage(),
																		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	0,
																		VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_IMAGE_LAYOUT_GENERAL,
																		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
																		static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImage(),
																		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				const VkExtent2D sourceExtent{ static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetExtent() };
				const VkExtent3D extent{ sourceExtent.width, sourceExtent.height, 0 };

				currentPrimaryCommandBuffer->CommandCopyImage(static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImage(), static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImage(), extent);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
																		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
																		VK_IMAGE_LAYOUT_GENERAL,
																		static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImage(),
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_ACCESS_SHADER_READ_BIT,
																		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
																		VK_IMAGE_LAYOUT_GENERAL,
																		static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate))->GetImage(),
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);
			}

			if (_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._ShouldClear)
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear(	currentStage == RenderPassMainStage::DirectionalShadowMapping ? 1.0f : 0.0f,
																				_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._RenderPass->Get(),
																				_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._FrameBuffers[renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameBufferIndex() : 0]->Get(),
																				renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution()._Width, renderPass->GetRenderResolution()._Height },
																				VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, _VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._NumberOfAttachments);
			}

			else
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPass(_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._RenderPass->Get(),
				_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._FrameBuffers[renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameBufferIndex() : 0]->Get(),
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
			currentPrimaryCommandBuffer->CommandExecuteCommands(reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(renderPass->GetCurrentCommandBuffer()->GetCommandBufferData())->Get());
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
	VulkanInterface::Instance->GetGraphicsQueue()->Submit(*_FrameData.GetCurrentPrimaryCommandBuffer(), 1, _Semaphores[UNDERLYING(GraphicsSemaphore::ImageAvailable)], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 1, _Semaphores[UNDERLYING(GraphicsSemaphore::RenderFinished)], _FrameData.GetCurrentFence()->Get());
}
#endif