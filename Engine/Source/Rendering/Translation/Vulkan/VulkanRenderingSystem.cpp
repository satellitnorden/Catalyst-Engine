//Header file.
#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/PointLightEntity.h>
#include <Entities/SpotLightEntity.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/TextureData.h>
#include <Rendering/Engine/RenderPasses/RenderPasses.h>
#include <Rendering/ShaderData/Vulkan/VulkanShaderData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationCommandBuffer.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Systems.
#include <Systems/EngineSystem.h>
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
	_FrameData.Initialize(	VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages(),
							*static_cast<VulkanDescriptorSetLayout *const RESTRICT>(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData)));
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

	//Update the dynamic uniform data.
	UpdateDynamicUniformData();
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
*	Returns the current frame index.
*/
uint8 VulkanRenderingSystem::GetCurrentFrameIndex() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();
}

/*
*	Creates a constant buffer.
*/
ConstantBufferHandle VulkanRenderingSystem::CreateConstantBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *dataSizes, const uint8 dataChunks) const NOEXCEPT
{
	return reinterpret_cast<ConstantBufferHandle>(VulkanInterface::Instance->CreateConstantBuffer(data, reinterpret_cast<const VkDeviceSize *const RESTRICT>(dataSizes), dataChunks));
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
void VulkanRenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, const TextureFilter filter, const AddressMode addressMode, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render target.
	*handle = static_cast<RenderTargetHandle>(VulkanInterface::Instance->CreateRenderTarget(	VulkanTranslationUtilities::GetVulkanExtent(resolution),
																								VulkanTranslationUtilities::GetVulkanFormat(format),
																								VulkanTranslationUtilities::GetVulkanTextureFilter(filter),
																								VulkanTranslationUtilities::GetVulkanAddressMode(addressMode)));
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
	parameters._Subpass = VulkanTranslationUtilities::GetSubStageIndex(renderPass->GetMainStage(), renderPass->GetSubStage());

	if (parameters._Subpass > 10)
	{
		BREAKPOINT();
	}

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
	parameters._ViewportExtent = renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution()._Width, renderPass->GetRenderResolution()._Height };

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
		VulkanCommandBuffer pipelineCommandBuffer;
		pipelineCommandPool->AllocateSecondaryCommandBuffer(pipelineCommandBuffer);
		renderPass->AddCommandBuffer(new (MemoryUtilities::GlobalPoolAllocate<sizeof(VulkanTranslationCommandBuffer)>()) VulkanTranslationCommandBuffer(pipelineCommandBuffer));
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

		vulkanBindings.EmplaceFast(VulkanUtilities::CreateDescriptorSetLayoutBinding(binding._Binding, VulkanTranslationUtilities::GetVulkanDescriptorType(binding._Type), VulkanTranslationUtilities::GetVulkanShaderStages(binding._ShaderStage)));
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

		case RenderDataTableUpdateInformation::Type::TextureCube:
		{
			writeDescriptorSet = static_cast<VulkanCubeMapTexture *const RESTRICT>(information._Handle)->GetWriteDescriptorSet(*descriptorSet, information._Binding);

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
#if defined(CATALYST_ENABLE_OCEAN)
	{
		//Initialize the above ocean fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetAboveOceanFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::AboveOceanFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the below ocean fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBelowOceanFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::BelowOceanFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
#endif

	{
		//Initialize the bloom fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetBloomFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::BloomFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
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
		//Initialize directional shadow instanced physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowInstancedPhysicalVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DirectionalInstancedPhysicalShadowVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetDirectionalShadowTerrainTessellationEvaluationShaderData(data);
		_ShaderModules[UNDERLYING(Shader::DirectionalTerrainShadowTessellationEvaluation)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}

	{
		//Initialize directional shadow terrain tesselation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetGaussianBlurFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::GaussianBlurFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the high detail terrain tessellation control shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTerrainTessellationControlShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTerrainTessellationControl)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
	}

	{
		//Initialize the high detail terrain tessellation evaluation shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTerrainTessellationEvaluationShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTerrainTessellationEvaluation)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	}

	{
		//Initialize the high detail terrain vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetHighDetailTerrainVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::HighDetailTerrainVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the instanced physical vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetInstancedPhysicalVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::InstancedPhysicalVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the lighting fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetLightingFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::LightingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
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
		//Initialize the post processing bloom fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPostProcessingBloomFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::PostProcessingBloomFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the post processing fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetPostProcessingFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::PostProcessingFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
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
		//Initialize the terrain scene buffer fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetTerrainFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::TerrainFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	
	{
		//Initialize the vegetation color fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationColorFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::VegetationColorFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the vegetation color vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationColorVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::VegetationColorVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the vegetation depth fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationDepthFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::VegetationDepthFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		//Initialize the vegetation depth vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVegetationDepthVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::VegetationDepthVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

	{
		//Initialize the viewport vertex shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetViewportVertexShaderData(data);
		_ShaderModules[UNDERLYING(Shader::ViewportVertex)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_VERTEX_BIT);
	}

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	{
		//Initialize the volumetric fog fragment shader module.
		DynamicArray<byte> data;
		VulkanShaderData::GetVolumetricFogFragmentShaderData(data);
		_ShaderModules[UNDERLYING(Shader::VolumetricFogFragment)] = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), VK_SHADER_STAGE_FRAGMENT_BIT);
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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadow)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadow)]._RenderPass->Get();
		
		StaticArray<VkImageView, 2> attachments
		{
			static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::DirectionalLight))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadowMap))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution, EngineSystem::Instance->GetProjectConfiguration()._RenderingConfiguration._ShadowMapResolution };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadow)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadow)]._FrameBuffers.EmplaceFast( VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadow)]._NumberOfAttachments = 2;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::DirectionalShadow)]._ShouldClear = true;
	}

	//Initialize the scene buffer render pass.
	{
		constexpr uint64 NUMBER_OF_SCENE_BUFFER_SUBPASSES{ 10
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
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),
			
			//Albedo.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferAlbedo))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Normal depth.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Material properties.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferMaterialProperties))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Directional shadow.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadow))->GetFormat(),
															VK_ATTACHMENT_LOAD_OP_CLEAR,
															VK_ATTACHMENT_STORE_OP_STORE,
															VK_ATTACHMENT_LOAD_OP_DONT_CARE,
															VK_ATTACHMENT_STORE_OP_DONT_CARE,
															VK_IMAGE_LAYOUT_UNDEFINED,
															VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),

			//Scene.
			VulkanUtilities::CreateAttachmentDescription(	static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetFormat(),
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

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
		subpassDescriptions[10] = VulkanUtilities::CreateSubpassDescription(	1,
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
														VK_DEPENDENCY_BY_REGION_BIT),

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
			VulkanUtilities::CreateSubpassDependency(	9,
														10,
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
														VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
														VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
														VK_DEPENDENCY_BY_REGION_BIT),
#endif
		};

		renderPassParameters._SubpassDependencyCount = static_cast<uint32>(subpassDependencies.Size());
		renderPassParameters._SubpassDependencies = subpassDependencies.Data();

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffer.
		VulkanFramebufferCreationParameters framebufferParameters;

		framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._RenderPass->Get();

		StaticArray<VkImageView, 6> attachments
		{
			static_cast<VulkanDepthBuffer *const RESTRICT>(RenderingSystem::Instance->GetDepthBuffer(DepthBuffer::SceneBuffer))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferAlbedo))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferMaterialProperties))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadow))->GetImageView(),
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._NumberOfAttachments = 6;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Scene)]._ShouldClear = true;
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

#if defined(CATALYST_ENABLE_OCEAN)
	//Initialize the ocean render pass.
	{
		constexpr uint64 NUMBER_OF_OCEAN_SUBPASSES{ 2 };

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
			VulkanUtilities::CreateSubpassDescription(	0,
														nullptr,
														1,
														colorAttachmentReferences.Data(),
														nullptr,
														0,
														nullptr),

			VulkanUtilities::CreateSubpassDescription(	0,
														nullptr,
														1,
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

		StaticArray<VkImageView, 1> attachments
		{
			static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImageView()
		};

		framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
		framebufferParameters._Attachments = attachments.Data();
		framebufferParameters._Extent = { RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height };

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._FrameBuffers.Reserve(1);
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::Ocean)]._ShouldClear = false;
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

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::PostProcessingFinal)]._RenderPass = VulkanInterface::Instance->CreateRenderPass(renderPassParameters);

		//Create the framebuffers.
		const DynamicArray<VkImageView> &swapchainImages{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::PostProcessingFinal)]._FrameBuffers.Reserve(swapchainImages.Size());

		for (VkImageView swapchainImage : swapchainImages)
		{
			VulkanFramebufferCreationParameters framebufferParameters;

			framebufferParameters._RenderPass = _VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::PostProcessingFinal)]._RenderPass->Get();

			framebufferParameters._AttachmentCount = 1;
			framebufferParameters._Attachments = &swapchainImage;
			framebufferParameters._Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

			_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::PostProcessingFinal)]._FrameBuffers.EmplaceFast(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
		}

		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::PostProcessingFinal)]._NumberOfAttachments = 1;
		_VulkanRenderPassMainStageData[UNDERLYING(RenderPassMainStage::PostProcessingFinal)]._ShouldClear = false;
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

		if (_DestructionQueue[i]._Frames > VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages())
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

#if defined(CATALYST_ENABLE_OCEAN)
			//Specialization - Copy scene to scene intermediate for the ocean render pass.
			if (currentStage == RenderPassMainStage::Ocean)
			{
				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
																		VK_ACCESS_TRANSFER_READ_BIT,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
																		static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImage(),
																		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	0,
																		VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
																		static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneIntermediate))->GetImage(),
																		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				const VkExtent2D sourceExtent{ static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetExtent() };
				const VkExtent3D extent{ sourceExtent.width, sourceExtent.height, 0 };

				currentPrimaryCommandBuffer->CommandCopyImage(static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImage(), static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneIntermediate))->GetImage(), extent);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
																		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene))->GetImage(),
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);

				currentPrimaryCommandBuffer->CommandPipelineBarrier(	VK_ACCESS_TRANSFER_WRITE_BIT,
																		VK_ACCESS_SHADER_READ_BIT,
																		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																		static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneIntermediate))->GetImage(),
																		VK_PIPELINE_STAGE_TRANSFER_BIT,
																		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
																		VK_DEPENDENCY_BY_REGION_BIT);
			}
#endif

			if (_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._ShouldClear)
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear(	_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._RenderPass->Get(),
																				_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._FrameBuffers[renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameIndex() : 0]->Get(),
																				renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ renderPass->GetRenderResolution()._Width, renderPass->GetRenderResolution()._Height },
																				VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, _VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._NumberOfAttachments);
			}

			else
			{
				currentPrimaryCommandBuffer->CommandBeginRenderPass(	_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._RenderPass->Get(),
																		_VulkanRenderPassMainStageData[UNDERLYING(currentStage)]._FrameBuffers[renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? GetCurrentFrameIndex() : 0]->Get(),
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
	VulkanInterface::Instance->GetGraphicsQueue()->Submit(*_FrameData.GetCurrentPrimaryCommandBuffer(), 1, _Semaphores[UNDERLYING(GraphicsSemaphore::ImageAvailable)], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 1, _Semaphores[UNDERLYING(GraphicsSemaphore::RenderFinished)], _FrameData.GetCurrentFence()->Get());
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

	_DynamicUniformData._CameraFieldOfViewCosine = CatalystBaseMath::CosineRadians(activeCamera->GetFieldOfViewRadians()) - 0.2f;
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
	_DynamicUniformData._WindSpeed = PhysicsSystem::Instance->GetWindSpeed();
	_DynamicUniformData._WindDirection = PhysicsSystem::Instance->GetWindDirection();

	//Upload the dynamic uniform data to the uniform buffer.
	_FrameData.GetCurrentDynamicUniformDataBuffer()->UploadData(static_cast<void*>(&_DynamicUniformData));
}