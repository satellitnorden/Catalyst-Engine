#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Systems/RenderingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//File.
#include <File/Core/BinaryFile.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/Vertex.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPassManager.h>
#include <Rendering/Translation/Vulkan/VulkanFrameData.h>
#include <Rendering/Translation/Vulkan/VulkanComputePipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanGraphicsPipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanRayTracingPipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Resources.
#include <Resources/Loading/TextureCubeData.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

//Vulkan rendering system data.
namespace VulkanRenderingSystemData
{
	/*
	*	Vulkan destruction data definition.
	*/
	class VulkanDestructionData final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			ACCELERATION_STRUCTURE,
			BUFFER,
			RENDER_DATA_TABLE,
			TEXTURE_2D
		};

		//The number of frames since destruction was requested.
		uint8 _Frames{ 0 };

		//The type.
		Type _Type;

		//The handle.
		OpaqueHandle _Handle;

		/*
		*	Constructor taking all values as arguments.
		*/
		VulkanDestructionData(const Type initial_type, OpaqueHandle initial_handle) NOEXCEPT
			:
			_Type(initial_type),
			_Handle(initial_handle)
		{

		}

	};

	//The Vulkan frame data.
	VulkanFrameData _FrameData;

	//The destruction queue.
	DynamicArray<VulkanDestructionData> _DestructionQueue;
}

//Vulkan rendering system logic.
namespace VulkanRenderingSystemLogic
{

	/*
	*	Concatenates all secondary command buffers into the previous one.
	*/
	void ConcatenateCommandBuffers() NOEXCEPT
	{
		//Begin the current primary command buffer.
		VulkanCommandBuffer *const RESTRICT currentPrimaryCommandBuffer{ VulkanRenderingSystemData::_FrameData.GetCurrentPrimaryCommandBuffer() };
		currentPrimaryCommandBuffer->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Iterate over all render passes and concatenate their command buffers into the primary command buffer.
		for (const RenderPass *const RESTRICT renderPass : *RenderPassManager::GetRenderPasses())
		{
			if (!renderPass->IsEnabled())
			{
				continue;
			}

			for (const Pipeline *const RESTRICT pipeline : renderPass->GetPipelines())
			{
				if (!pipeline->IncludeInRender())
				{
					continue;
				}

				if (pipeline->GetType() == Pipeline::Type::Graphics)
				{
					//Cache the Vulkan graphics pipeline data.
					const VulkanGraphicsPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetData()) };

					if (static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->ShouldClear())
					{
						currentPrimaryCommandBuffer->CommandBeginRenderPassAndClear(Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f),
																					0.0f,
																					pipelineData->_RenderPass->Get(),
																					pipelineData->_FrameBuffers[0]->Get(),
																					pipelineData->_Extent,
																					VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, pipelineData->_NumberOfAttachments);
					}

					else
					{
						currentPrimaryCommandBuffer->CommandBeginRenderPass(pipelineData->_RenderPass->Get(),
																			pipelineData->_FrameBuffers[pipelineData->_RenderToScreeen ? RenderingSystem::Instance->GetCurrentFramebufferIndex() : 0]->Get(),
																			pipelineData->_RenderToScreeen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : pipelineData->_Extent,
																			VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
					}
				}

				currentPrimaryCommandBuffer->CommandExecuteCommands(reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(pipeline->GetCurrentCommandBuffer()->GetCommandBufferData())->Get());
			
				if (pipeline->GetType() == Pipeline::Type::Graphics)
				{
					currentPrimaryCommandBuffer->CommandEndRenderPass();
				}

				//Insert full pipeline barrier.
				if (false)
				{
					//Create the image memory barrier.
					VkMemoryBarrier memory_barrier{ };

					memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
					memory_barrier.pNext = nullptr;
					memory_barrier.srcAccessMask =	VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
													VK_ACCESS_INDEX_READ_BIT |
													VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
													VK_ACCESS_UNIFORM_READ_BIT |
													VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
													VK_ACCESS_SHADER_READ_BIT |
													VK_ACCESS_SHADER_WRITE_BIT |
													VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
													VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
													VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
													VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
													VK_ACCESS_TRANSFER_READ_BIT |
													VK_ACCESS_TRANSFER_WRITE_BIT |
													VK_ACCESS_HOST_READ_BIT |
													VK_ACCESS_HOST_WRITE_BIT;
					memory_barrier.dstAccessMask =	VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
													VK_ACCESS_INDEX_READ_BIT |
													VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
													VK_ACCESS_UNIFORM_READ_BIT |
													VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
													VK_ACCESS_SHADER_READ_BIT |
													VK_ACCESS_SHADER_WRITE_BIT |
													VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
													VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
													VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
													VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
													VK_ACCESS_TRANSFER_READ_BIT |
													VK_ACCESS_TRANSFER_WRITE_BIT |
													VK_ACCESS_HOST_READ_BIT |
													VK_ACCESS_HOST_WRITE_BIT;

					//Record the pipeline barrier command.
					vkCmdPipelineBarrier(currentPrimaryCommandBuffer->Get(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &memory_barrier, 0, nullptr, 0, nullptr);
				}

				//Insert image barriers.
				if (true)
				{
					if (pipeline->GetType() == Pipeline::Type::Graphics)
					{
						for (const RenderTargetHandle render_target : static_cast<const GraphicsPipeline* const RESTRICT>(pipeline)->GetRenderTargets())
						{
							if (render_target == RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCREEN))
							{
								continue;
							}

							VulkanRenderTarget* const RESTRICT vulkan_render_target{ static_cast<VulkanRenderTarget* const RESTRICT>(render_target) };

							//Create the image memory barrier.
							VkImageMemoryBarrier imageMemoryBarrier{ };

							imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
							imageMemoryBarrier.pNext = nullptr;
							imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
							imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

							imageMemoryBarrier.oldLayout = vulkan_render_target->GetImageLayout();
							imageMemoryBarrier.newLayout = vulkan_render_target->GetImageLayout();
							imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
							imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
							imageMemoryBarrier.image = vulkan_render_target->GetImage();

							imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
							imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
							imageMemoryBarrier.subresourceRange.levelCount = 1;
							imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
							imageMemoryBarrier.subresourceRange.layerCount = 1;

							//Record the pipeline barrier command.
							vkCmdPipelineBarrier(currentPrimaryCommandBuffer->Get(), VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
						}
					}
				}
			}
		}
	}

	/*
	*	Initializes a compute pipeline.
	*/
	void InitializeComputePipeline(ComputePipeline *const RESTRICT pipeline) NOEXCEPT
	{
		//Create the pipeline.
		VulkanComputePipelineCreationParameters parameters;

		DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
		pipelineDescriptorSetLayouts.Reserve(pipeline->GetRenderDataTableLayouts().Size());

		for (RenderDataTableLayoutHandle renderDataTableLayout : pipeline->GetRenderDataTableLayouts())
		{
			pipelineDescriptorSetLayouts.Emplace(*static_cast<VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
		}

		parameters._DescriptorSetLayoutCount = static_cast<uint32>(pipelineDescriptorSetLayouts.Size());
		parameters._DescriptorSetLayouts = pipelineDescriptorSetLayouts.Data();

		DynamicArray<VkPushConstantRange> pushConstantRanges;

		if (pipeline->GetPushConstantRanges().Empty())
		{
			parameters._PushConstantRangeCount = 0;
			parameters._PushConstantRanges = nullptr;
		}

		else
		{
			pushConstantRanges.Reserve(pipeline->GetPushConstantRanges().Size());

			for (const PushConstantRange &pushConstantRange : pipeline->GetPushConstantRanges())
			{
				pushConstantRanges.Emplace(VulkanTranslationUtilities::GetVulkanPushConstantRange(pushConstantRange));
			}

			parameters._PushConstantRangeCount = static_cast<uint32>(pushConstantRanges.Size());
			parameters._PushConstantRanges = pushConstantRanges.Data();
		}

		parameters._ShaderModule = static_cast<VulkanShaderModule *const RESTRICT>(pipeline->GetShader()->_Handle);

		//Create the pipeline sub stage data.
		VulkanComputePipelineData *const RESTRICT data{ new (Memory::GlobalLinearAllocator()->Allocate(sizeof(VulkanComputePipelineData))) VulkanComputePipelineData() };
		pipeline->SetData(data);

		//Create the pipeline!
		data->_Pipeline = VulkanInterface::Instance->CreateComputePipeline(parameters);
	}

	/*
	*	Initializes a graphics pipeline.
	*/
	void InitializeGraphicsPipeline(GraphicsPipeline *const RESTRICT pipeline) NOEXCEPT
	{
		//Create the graphics pipeline data.
		VulkanGraphicsPipelineData *const RESTRICT data{ new (Memory::GlobalLinearAllocator()->Allocate(sizeof(VulkanGraphicsPipelineData))) VulkanGraphicsPipelineData() };
		pipeline->SetData(data);

		{
			//Create the vulkan render pass.
			VulkanRenderPassCreationParameters parameters;

			//Determine the attachments that will be used by the pipelines in this render pass.
			Map<RenderTargetHandle, uint32> uniqueAttachments;
			uint32 attachmentCounter{ 0 };

			for (const RenderTargetHandle renderTarget : pipeline->GetRenderTargets())
			{
				if (!uniqueAttachments.Find(renderTarget))
				{
					uniqueAttachments.Emplace(renderTarget, attachmentCounter++);
				}
			}

			//Cache the depth buffer.
			VulkanDepthBuffer *const RESTRICT depthBuffer{ static_cast<VulkanDepthBuffer *const RESTRICT>(pipeline->GetDepthBuffer()) };

			//Create the attachment descriptions.
			DynamicArray<VkAttachmentDescription> attachmentDescriptions;
			attachmentDescriptions.Reserve(uniqueAttachments.Size() + (depthBuffer ? 1 : 0));

			DynamicArray<VkAttachmentReference> colorAttachmentReferences;
			colorAttachmentReferences.Reserve(uniqueAttachments.Size());

			VkAttachmentReference depthStencilAttachmentReference;

			uint32 counter{ 0 };

			if (depthBuffer)
			{
				attachmentDescriptions.Emplace(VulkanUtilities::CreateAttachmentDescription(depthBuffer->GetFormat(),
																								pipeline->ShouldClear() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
																								VK_ATTACHMENT_STORE_OP_STORE,
																								pipeline->ShouldClear() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
																								VK_ATTACHMENT_STORE_OP_STORE,
																								VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
																								VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));

				depthStencilAttachmentReference = VkAttachmentReference{ counter++, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
			}

			for (const Pair<RenderTargetHandle, uint32> uniqueAttachment : uniqueAttachments)
			{
				attachmentDescriptions.Emplace(VulkanUtilities::CreateAttachmentDescription(uniqueAttachment._First == RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCREEN) ? VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format : static_cast<VulkanRenderTarget *const RESTRICT>(uniqueAttachment._First)->GetFormat(),
																								pipeline->ShouldClear() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																								VK_ATTACHMENT_STORE_OP_STORE,
																								VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																								VK_ATTACHMENT_STORE_OP_DONT_CARE,
																								uniqueAttachment._First == RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCREEN) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_GENERAL,
																								uniqueAttachment._First == RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCREEN) ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_GENERAL));

				colorAttachmentReferences.Emplace(VkAttachmentReference{ counter++, uniqueAttachment._First == RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCREEN) ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL });
			}

			parameters._AttachmentCount = static_cast<uint32>(attachmentDescriptions.Size());
			parameters._AttachmentDescriptions = attachmentDescriptions.Data();

			//Create the subpass descriptions.
			VkSubpassDescription subpassDescription{ VulkanUtilities::CreateSubpassDescription(	0,
																								nullptr,
																								static_cast<uint32>(colorAttachmentReferences.Size()),
																								colorAttachmentReferences.Data(),
																								depthBuffer ? &depthStencilAttachmentReference : nullptr,
																								0,
																								nullptr) };

			parameters._SubpassDescriptionCount = 1;
			parameters._SubpassDescriptions = &subpassDescription;

			//Create the subpass depencies.
			StaticArray<VkSubpassDependency, 1> dependencies;

			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dstAccessMask =	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
											| VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = 0;

			/*
			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = 0;
			dependencies[1].srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
											| VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dependencyFlags = 0;
			*/

			parameters._SubpassDependencyCount = static_cast<uint32>(dependencies.Size());
			parameters._SubpassDependencies = dependencies.Data();

			//Create the render pass!
			data->_RenderPass = VulkanInterface::Instance->CreateRenderPass(parameters);

			//Create the framebuffer(s).
			if (!uniqueAttachments.Empty() && uniqueAttachments.begin()->_First == RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCREEN))
			{
				//Create the framebuffers.
				const DynamicArray<VkImageView> &swapchainImages{ VulkanInterface::Instance->GetSwapchain().GetSwapChainImageViews() };
				data->_FrameBuffers.Reserve(swapchainImages.Size());

				for (VkImageView swapchainImage : swapchainImages)
				{
					VulkanFramebufferCreationParameters framebufferParameters;

					framebufferParameters._RenderPass = data->_RenderPass->Get();
					framebufferParameters._AttachmentCount = 1;
					framebufferParameters._Attachments = &swapchainImage;
					framebufferParameters._Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();

					data->_FrameBuffers.Emplace(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
				}

				data->_Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
				data->_NumberOfAttachments = static_cast<uint32>(1);
				data->_RenderToScreeen = true;
			}

			else
			{
				VulkanFramebufferCreationParameters framebufferParameters;

				framebufferParameters._RenderPass = data->_RenderPass->Get();

				DynamicArray<VkImageView> attachments;
				attachments.Reserve(uniqueAttachments.Size() + (depthBuffer ? 1 : 0));

				if (depthBuffer)
				{
					attachments.Emplace(depthBuffer->GetImageView());
				}

				for (const Pair<RenderTargetHandle, uint32> uniqueAttachment : uniqueAttachments)
				{
					attachments.Emplace(static_cast<VulkanRenderTarget *const RESTRICT>(uniqueAttachment._First)->GetImageView());
				}

				framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
				framebufferParameters._Attachments = attachments.Data();
				framebufferParameters._Extent = { pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height };

				data->_FrameBuffers.Reserve(1);
				data->_FrameBuffers.Emplace(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
				data->_Extent = { pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height };
				data->_NumberOfAttachments = static_cast<uint32>(attachments.Size());
				data->_RenderToScreeen = false;
			}
		}

		{
			//Create the pipeline.
			VulkanGraphicsPipelineCreationParameters parameters;

			parameters._BlendEnable = pipeline->IsBlendEnabled();
			parameters._BlendFactorSourceColor = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorSourceColor());
			parameters._BlendFactorDestinationColor = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorDestinationColor());
			parameters._BlendFactorSourceAlpha = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorSourceAlpha());
			parameters._BlendFactorDestinationAlpha = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorDestinationAlpha());
			parameters._ColorAttachmentCount = static_cast<uint32>(pipeline->GetRenderTargets().Size());
			parameters._CullMode = VulkanTranslationUtilities::GetVulkanCullMode(pipeline->GetCullMode());
			parameters._DepthCompareOp = VulkanTranslationUtilities::GetVulkanCompareOperator(pipeline->GetDepthCompareOperator());
			parameters._DepthTestEnable = pipeline->IsDepthTestEnabled();
			parameters._DepthWriteEnable = pipeline->IsDepthWriteEnabled();

			DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
			pipelineDescriptorSetLayouts.Reserve(pipeline->GetRenderDataTableLayouts().Size());

			for (RenderDataTableLayoutHandle renderDataTableLayout : pipeline->GetRenderDataTableLayouts())
			{
				pipelineDescriptorSetLayouts.Emplace(*static_cast<VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
			}

			parameters._DescriptorSetLayoutCount = static_cast<uint32>(pipelineDescriptorSetLayouts.Size());
			parameters._DescriptorSetLayouts = pipelineDescriptorSetLayouts.Data();

			DynamicArray<VkPushConstantRange> pushConstantRanges;

			if (pipeline->GetPushConstantRanges().Empty())
			{
				parameters._PushConstantRangeCount = 0;
				parameters._PushConstantRanges = nullptr;
			}

			else
			{
				pushConstantRanges.Reserve(pipeline->GetPushConstantRanges().Size());

				for (const PushConstantRange &pushConstantRange : pipeline->GetPushConstantRanges())
				{
					pushConstantRanges.Emplace(VulkanTranslationUtilities::GetVulkanPushConstantRange(pushConstantRange));
				}

				parameters._PushConstantRangeCount = static_cast<uint32>(pushConstantRanges.Size());
				parameters._PushConstantRanges = pushConstantRanges.Data();
			}

			parameters._ShaderModules.Reserve(5);
			if (pipeline->GetVertexShader()) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetVertexShader()->_Handle));
			if (pipeline->GetTessellationControlShader()) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetTessellationControlShader()->_Handle));
			if (pipeline->GetTessellationEvaluationShader()) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetTessellationEvaluationShader()->_Handle));
			if (pipeline->GetGeometryShader()) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetGeometryShader()->_Handle));
			if (pipeline->GetFragmentShader()) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetFragmentShader()->_Handle));
			
			parameters._StencilTestEnable = pipeline->IsStencilTestEnabled();
			parameters._StencilFailOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(pipeline->GetStencilFailOperator());
			parameters._StencilPassOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(pipeline->GetStencilPassOperator());
			parameters._StencilDepthFailOperator = VulkanTranslationUtilities::GetVulkanStencilOperator(pipeline->GetStencilDepthFailOperator());
			parameters._StencilCompareOperator = VulkanTranslationUtilities::GetVulkanCompareOperator(pipeline->GetStencilCompareOperator());
			parameters._StencilCompareMask = pipeline->GetStencilCompareMask();
			parameters._StencilWriteMask = pipeline->GetStencilWriteMask();
			parameters._StencilReferenceMask = pipeline->GetStencilReferenceMask();
			parameters._Subpass = 0;

			parameters._Topology = VulkanTranslationUtilities::GetVulkanTopology(pipeline->GetTopology());

			DynamicArray<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
			vertexInputAttributeDescriptions.Reserve(pipeline->GetVertexInputAttributeDescriptions().Size());

			for (const VertexInputAttributeDescription &vertexInputAttributeDescription : pipeline->GetVertexInputAttributeDescriptions())
			{
				vertexInputAttributeDescriptions.Emplace(VulkanTranslationUtilities::GetVulkanVertexInputAttributeDescription(vertexInputAttributeDescription));
			}

			parameters._VertexInputAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.Size());
			parameters._VertexInputAttributeDescriptions = vertexInputAttributeDescriptions.Data();

			DynamicArray<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
			vertexInputBindingDescriptions.Reserve(pipeline->GetVertexInputBindingDescriptions().Size());

			for (const VertexInputBindingDescription &vertexInputBindingDescription : pipeline->GetVertexInputBindingDescriptions())
			{
				vertexInputBindingDescriptions.Emplace(VulkanTranslationUtilities::GetVulkanVertexInputBindingDescription(vertexInputBindingDescription));
			}

			parameters._VertexInputBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.Size());
			parameters._VertexInputBindingDescriptions = vertexInputBindingDescriptions.Data();
			parameters._ViewportExtent = pipeline->GetRenderTargets().Empty() ? VkExtent2D{ pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height } : pipeline->GetRenderTargets()[0] == RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCREEN) ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : VkExtent2D{ pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height };

			parameters._RenderPass = data->_RenderPass;

			//Create the pipeline!
			data->_Pipeline = VulkanInterface::Instance->CreateGraphicsPipeline(parameters);
		}
	}

	/*
	*	Initializes a ray tracing pipeline.
	*/
	void InitializeRayTracingPipeline(RayTracingPipeline *const RESTRICT pipeline) NOEXCEPT
	{
		//Create the pipeline.
		VulkanRayTracingPipelineCreationParameters parameters;

		DynamicArray<VulkanDescriptorSetLayout> pipelineDescriptorSetLayouts;
		pipelineDescriptorSetLayouts.Reserve(pipeline->GetRenderDataTableLayouts().Size());

		for (RenderDataTableLayoutHandle renderDataTableLayout : pipeline->GetRenderDataTableLayouts())
		{
			pipelineDescriptorSetLayouts.Emplace(*static_cast<VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
		}

		parameters._DescriptorSetLayoutCount = static_cast<uint32>(pipelineDescriptorSetLayouts.Size());
		parameters._DescriptorSetLayouts = pipelineDescriptorSetLayouts.Data();

		DynamicArray<VkPushConstantRange> pushConstantRanges;

		if (pipeline->GetPushConstantRanges().Empty())
		{
			parameters._PushConstantRangeCount = 0;
			parameters._PushConstantRanges = nullptr;
		}

		else
		{
			pushConstantRanges.Reserve(pipeline->GetPushConstantRanges().Size());

			for (const PushConstantRange &pushConstantRange : pipeline->GetPushConstantRanges())
			{
				pushConstantRanges.Emplace(VulkanTranslationUtilities::GetVulkanPushConstantRange(pushConstantRange));
			}

			parameters._PushConstantRangeCount = static_cast<uint32>(pushConstantRanges.Size());
			parameters._PushConstantRanges = pushConstantRanges.Data();
		}

		parameters._RayGenerationShaderModule = static_cast<VulkanShaderModule *const RESTRICT>(pipeline->GetRayGenerationShader()->_Handle);

		for (const RayTracingPipeline::HitGroup hit_group : pipeline->GetHitGroups())
		{
			parameters._HitGroups.Emplace(	hit_group._ClosestHitShader ? static_cast<const VulkanShaderModule *const RESTRICT>(hit_group._ClosestHitShader->_Handle) : nullptr,
											hit_group._AnyHitShader ? static_cast<const VulkanShaderModule *const RESTRICT>(hit_group._AnyHitShader->_Handle) : nullptr,
											hit_group._IntersectionShader ? static_cast<const VulkanShaderModule *const RESTRICT>(hit_group._IntersectionShader->_Handle) : nullptr);
		}

		for (const ResourcePointer<ShaderResource> shader : pipeline->GetMissShaders())
		{
			parameters._MissShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(shader->_Handle));
		}

		//Create the pipeline sub stage data.
		VulkanRayTracingPipelineData *const RESTRICT data{ new (Memory::GlobalLinearAllocator()->Allocate(sizeof(VulkanRayTracingPipelineData))) VulkanRayTracingPipelineData() };
		pipeline->SetData(data);

		//Create the pipeline!
		data->_Pipeline = VulkanInterface::Instance->CreateRayTracingPipeline(parameters);

		//Create the shader binding table buffer.
		const uint32 shader_group_handle_size{ VulkanInterface::Instance->GetPhysicalDevice().GetRayTracingProperties().shaderGroupHandleSize };

		//Calculate the shader binding table size.
		uint32 shader_binding_table_size{ shader_group_handle_size };

		for (const RayTracingPipeline::HitGroup hit_group : pipeline->GetHitGroups())
		{
			shader_binding_table_size += shader_group_handle_size;
		}

		for (const ResourcePointer<ShaderResource> shader : pipeline->GetMissShaders())
		{
			shader_binding_table_size += shader_group_handle_size;
		}

		//Create the shader binding table buffer.
		data->_ShaderBindingTableBuffer = VulkanInterface::Instance->CreateBuffer(shader_binding_table_size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		//Upload the data to it.
		uint8 *const RESTRICT shader_binding_table_storage{ static_cast<uint8 *const RESTRICT>(Memory::Allocate(shader_binding_table_size)) };

		VULKAN_ERROR_CHECK(vkGetRayTracingShaderGroupHandlesNV(	VulkanInterface::Instance->GetLogicalDevice().Get(),
																data->_Pipeline->GetPipeline(),
																0,
																shader_binding_table_size / shader_group_handle_size,
																shader_binding_table_size,
																shader_binding_table_storage));

		void *const RESTRICT data_chunks[]{ shader_binding_table_storage };
		const uint64 data_sizes[]{ shader_binding_table_size };

		data->_ShaderBindingTableBuffer->UploadData(data_chunks, data_sizes, 1);

		Memory::Free(shader_binding_table_storage);

		//Calculate the offsets/strides.
		data->_HitShaderBindingOffset = shader_group_handle_size;
		data->_HitShaderBindingStride = shader_group_handle_size;
		data->_MissShaderBindingOffset = shader_group_handle_size + static_cast<uint32>(pipeline->GetHitGroups().Size()) * shader_group_handle_size;
		data->_MissShaderBindingStride = shader_group_handle_size;
	}

	/*
	*	Processes the destruction queue.
	*/
	void ProcessDestructionQueue() NOEXCEPT
	{
		for (uint64 i = 0; i < VulkanRenderingSystemData::_DestructionQueue.Size();)
		{
			++VulkanRenderingSystemData::_DestructionQueue[i]._Frames;

			if (VulkanRenderingSystemData::_DestructionQueue[i]._Frames > VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() + 1)
			{
				switch (VulkanRenderingSystemData::_DestructionQueue[i]._Type)
				{
					case VulkanRenderingSystemData::VulkanDestructionData::Type::ACCELERATION_STRUCTURE:
					{
						VulkanInterface::Instance->DestroyAccelerationStructure(static_cast<VulkanAccelerationStructure *const RESTRICT>(VulkanRenderingSystemData::_DestructionQueue[i]._Handle));

						break;
					}

					case VulkanRenderingSystemData::VulkanDestructionData::Type::BUFFER:
					{
						VulkanInterface::Instance->DestroyBuffer(static_cast<VulkanBuffer *const RESTRICT>(VulkanRenderingSystemData::_DestructionQueue[i]._Handle));

						break;
					}

					case VulkanRenderingSystemData::VulkanDestructionData::Type::RENDER_DATA_TABLE:
					{
						VulkanInterface::Instance->DestroyDescriptorSet(static_cast<VulkanDescriptorSet *const RESTRICT>(VulkanRenderingSystemData::_DestructionQueue[i]._Handle));

						break;
					}

					case VulkanRenderingSystemData::VulkanDestructionData::Type::TEXTURE_2D:
					{
						VulkanInterface::Instance->Destroy2DTexture(static_cast<Vulkan2DTexture *const RESTRICT>(VulkanRenderingSystemData::_DestructionQueue[i]._Handle));

						break;
					}

					default:
					{
						ASSERT(false, "Unhandled case!");

						break;
					}
				}

				VulkanRenderingSystemData::_DestructionQueue.EraseAt(i);
			}

			else
			{
				++i;
			}
		}
	}

}

/*
*	Terminates the rendering system.
*/
void RenderingSystem::Terminate() NOEXCEPT
{
	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Returns whether or not ray tracing is supported.
*/
NO_DISCARD bool RenderingSystem::IsRayTracingSupported() const NOEXCEPT
{
	//Just check if the physical device has ray tracing support.
	return VulkanInterface::Instance->GetPhysicalDevice().HasRayTracingSupport();
}

/*
*	Returns the number of framebuffers.
*/
uint8 RenderingSystem::GetNumberOfFramebuffers() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages();
}

/*
*	Returns the current framebuffer index.
*/
uint8 RenderingSystem::GetCurrentFramebufferIndex() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();
}

/*
*	Creates a bottom level acceleration structure.
*/
void RenderingSystem::CreateBottomLevelAccelerationStructure(	const BufferHandle &vertexBuffer,
																const uint32 numberOfVertices,
																const BufferHandle &indexBuffer,
																const uint32 numberOfIndices,
																AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{
	if (IsRayTracingSupported())
	{
		VkGeometryNV geometry;

		geometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
		geometry.pNext = nullptr;
		geometry.geometryType = VkGeometryTypeNV::VK_GEOMETRY_TYPE_TRIANGLES_NV;
		geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
		geometry.geometry.triangles.pNext = nullptr;
		geometry.geometry.triangles.vertexData = static_cast<VulkanBuffer *const RESTRICT>(vertexBuffer)->Get();
		geometry.geometry.triangles.vertexOffset = 0;
		geometry.geometry.triangles.vertexCount = numberOfVertices;
		geometry.geometry.triangles.vertexStride = sizeof(Vertex);
		geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		geometry.geometry.triangles.indexData = static_cast<VulkanBuffer *const RESTRICT>(indexBuffer)->Get();
		geometry.geometry.triangles.indexOffset = 0;
		geometry.geometry.triangles.indexCount = numberOfIndices;
		geometry.geometry.triangles.indexType = VkIndexType::VK_INDEX_TYPE_UINT32;
		geometry.geometry.triangles.transformData = VK_NULL_HANDLE;
		geometry.geometry.triangles.transformOffset = 0;
		geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
		geometry.geometry.aabbs.pNext = nullptr;
		geometry.geometry.aabbs.aabbData = VK_NULL_HANDLE;
		geometry.geometry.aabbs.numAABBs = 0;
		geometry.geometry.aabbs.stride = 0;
		geometry.geometry.aabbs.offset = 0;
		geometry.flags = 0;

		*handle = VulkanInterface::Instance->CreateAccelerationStructure(	VkAccelerationStructureTypeNV::VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV,
																			ArrayProxy<VulkanGeometryInstance>(),
																			ArrayProxy<VkGeometryNV>(geometry));
	}

	else
	{
		*handle = EMPTY_HANDLE;
	}
}

/*
*	Creates a top level acceleration structure.
*/
void RenderingSystem::CreateTopLevelAccelerationStructure(const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instance_data, AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{
	if (IsRayTracingSupported())
	{
		//Create the Vulkan geometry instance structres.
		VulkanGeometryInstance* const RESTRICT geometry_instances{ MemorySystem::Instance->FrameAllocate<VulkanGeometryInstance>(sizeof(VulkanGeometryInstance) * instance_data.Size()) };

		for (uint64 i{ 0 }, size{ instance_data.Size() }; i < size; ++i)
		{
			VulkanTranslationUtilities::GetVulkanGeometryInstance(instance_data[i], &geometry_instances[i]);
		}

		//Create the acceleration structure.
		*handle = VulkanInterface::Instance->CreateAccelerationStructure(	VkAccelerationStructureTypeNV::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV,
																			ArrayProxy<VulkanGeometryInstance>(geometry_instances, instance_data.Size()),
																			ArrayProxy<VkGeometryNV>());
	}
	
	else
	{
		*handle = nullptr;
	}
}

/*
*	Destroys an acceleration structure.
*/
void RenderingSystem::DestroyAccelerationStructure(AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	VulkanRenderingSystemData::_DestructionQueue.Emplace(VulkanRenderingSystemData::VulkanDestructionData::Type::ACCELERATION_STRUCTURE, *handle);
}

/*
*	Creates a buffer.
*/
void RenderingSystem::CreateBuffer(const uint64 size, const BufferUsage usage, const MemoryProperty memoryProperties, BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the buffer.
	*handle = static_cast<BufferHandle>(VulkanInterface::Instance->CreateBuffer(size, VulkanTranslationUtilities::GetVulkanBufferUsage(usage), VulkanTranslationUtilities::GetVulkanMemoryProperty(memoryProperties)));
}

/*
*	Uploads data to a buffer.
*/
void RenderingSystem::UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT dataSizes, const uint8 dataChunks, BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Upload data to the buffer.
	static_cast<VulkanBuffer *const RESTRICT>(*handle)->UploadData(data, static_cast<const VkDeviceSize *const RESTRICT>(dataSizes), dataChunks);
}

/*
*	Destroys a buffer.
*/
void RenderingSystem::DestroyBuffer(BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	VulkanRenderingSystemData::_DestructionQueue.Emplace(VulkanRenderingSystemData::VulkanDestructionData::Type::BUFFER, *handle);
}

/*
*	Creates a command pool.
*/
void RenderingSystem::CreateCommandPool(const Pipeline::Type type, const CommandPoolCreateFlags flags, CommandPoolHandle *const RESTRICT handle) const NOEXCEPT
{
	//Retrieve the vulkan flags.
	VkCommandPoolCreateFlags vulkan_flags;

	if (TEST_BIT(flags, CommandPoolCreateFlags::RESET_COMMAND_BUFFER))
	{
		vulkan_flags |= VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	}

	switch (type)
	{
		case Pipeline::Type::Compute:
		case Pipeline::Type::RayTracing:
		{
			*handle = VulkanInterface::Instance->CreateComputeCommandPool(vulkan_flags);

			break;
		}

		case Pipeline::Type::Graphics:
		{
			*handle = VulkanInterface::Instance->CreateGraphicsCommandPool(vulkan_flags);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Allocates a command buffer from the given command pool.
*/
RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT RenderingSystem::AllocateCommandBuffer(const CommandPoolHandle command_pool, const CommandBufferLevel level) const NOEXCEPT
{
	//Cache the vulkan command pool.
	VulkanCommandPool *const RESTRICT vulkan_command_pool{ static_cast<VulkanCommandPool *const RESTRICT>(command_pool) };

	//Allocate and initialize the Vulkan command buffer.
	VulkanCommandBuffer *const RESTRICT vulkan_command_buffer{ new (MemorySystem::Instance->TypeAllocate<VulkanCommandBuffer>()) VulkanCommandBuffer };

	switch (level)
	{
		case CommandBufferLevel::PRIMARY:
		{
			vulkan_command_pool->AllocatePrimaryCommandBuffer(*vulkan_command_buffer);

			break;
		}

		case CommandBufferLevel::SECONDARY:
		{
			vulkan_command_pool->AllocateSecondaryCommandBuffer(*vulkan_command_buffer);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
	
	//Allocate and initialize the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ new (MemorySystem::Instance->TypeAllocate<CommandBuffer>()) CommandBuffer(vulkan_command_buffer) };
	
	//Return the command buffer.
	return command_buffer;
}

/*
*	Creates a depth buffer.
*/
void RenderingSystem::CreateDepthBuffer(const Resolution resolution, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the depth buffer.
	*handle = static_cast<DepthBufferHandle>(VulkanInterface::Instance->CreateDepthBuffer(VkExtent2D{ resolution._Width, resolution._Height }));
}

/*
*	Creates a Shader.
*/
void RenderingSystem::CreateShader(const ArrayProxy<byte> &data, const ShaderStage stage, ShaderHandle *const RESTRICT handle) const NOEXCEPT
{
	VkShaderStageFlagBits vulkan_stage;

	switch (stage)
	{
		case ShaderStage::COMPUTE:
		{
			vulkan_stage = VK_SHADER_STAGE_COMPUTE_BIT;

			break;
		}

		case ShaderStage::FRAGMENT:
		{
			vulkan_stage = VK_SHADER_STAGE_FRAGMENT_BIT;

			break;
		}

		case ShaderStage::GEOMETRY:
		{
			vulkan_stage = VK_SHADER_STAGE_GEOMETRY_BIT;

			break;
		}

		case ShaderStage::RAY_ANY_HIT:
		{
			vulkan_stage = VK_SHADER_STAGE_ANY_HIT_BIT_NV;

			break;
		}

		case ShaderStage::RAY_CLOSEST_HIT:
		{
			vulkan_stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;

			break;
		}

		case ShaderStage::RAY_GENERATION:
		{
			vulkan_stage = VK_SHADER_STAGE_RAYGEN_BIT_NV;

			break;
		}

		case ShaderStage::RAY_INTERSECTION:
		{
			vulkan_stage = VK_SHADER_STAGE_INTERSECTION_BIT_NV;

			break;
		}

		case ShaderStage::RAY_MISS:
		{
			vulkan_stage = VK_SHADER_STAGE_MISS_BIT_NV;

			break;
		}

		case ShaderStage::TESSELLATION_CONTROL:
		{
			vulkan_stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

			break;
		}

		case ShaderStage::TESSELLATION_EVALUATION:
		{
			vulkan_stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

			break;
		}

		case ShaderStage::VERTEX:
		{
			vulkan_stage = VK_SHADER_STAGE_VERTEX_BIT;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	*handle = VulkanInterface::Instance->CreateShaderModule(data.Data(), data.Size(), vulkan_stage);
}

/*
*	Creates a render data table layout.
*/
void RenderingSystem::CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 numberOfBindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	//Translate from the API-agnostic structure to the Vulkan-specific.
	DynamicArray<VkDescriptorSetLayoutBinding> vulkanBindings;
	vulkanBindings.Reserve(numberOfBindings);

	for (uint32 i = 0; i < numberOfBindings; ++i)
	{
		const RenderDataTableLayoutBinding &binding{ bindings[i] };

		vulkanBindings.Emplace(VulkanUtilities::CreateDescriptorSetLayoutBinding(binding._Binding, VulkanTranslationUtilities::GetVulkanDescriptorType(binding._Type), binding._NumberOfArrayElements, VulkanTranslationUtilities::GetVulkanShaderStages(binding._ShaderStage)));
	}

	*handle = VulkanInterface::Instance->CreateDescriptorSetLayout(vulkanBindings.Data(), numberOfBindings);
}

/*
*	Creates a render data table.
*/
void RenderingSystem::CreateRenderDataTable(const RenderDataTableLayoutHandle renderDataTableLayout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	*handle = VulkanInterface::Instance->CreateDescriptorSet(*static_cast<const VulkanDescriptorSetLayout *const RESTRICT>(renderDataTableLayout));
}

/*
*	Binds an acceleration structure to a render data table.
*/
void RenderingSystem::BindAccelerationStructureToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, AccelerationStructureHandle accelerationStructure) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanAccelerationStructure *const RESTRICT vulkanAccelerationStructure{ static_cast<VulkanAccelerationStructure *const RESTRICT>(accelerationStructure) };

	//Create the acceleration structure write descriptor set.
	VkWriteDescriptorSetAccelerationStructureNV accelerationStructureWriteDescriptorSet;

	accelerationStructureWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV;
	accelerationStructureWriteDescriptorSet.pNext = nullptr;
	accelerationStructureWriteDescriptorSet.accelerationStructureCount = 1;
	accelerationStructureWriteDescriptorSet.pAccelerationStructures = &vulkanAccelerationStructure->GetAccelerationStructure();

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = &accelerationStructureWriteDescriptorSet;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
	writeDescriptorSet.pImageInfo = nullptr;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a combined image sampler to a render data table.
*	Accepts render target, texture 2D and texture cube handles.
*/
void RenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, OpaqueHandle image, SamplerHandle sampler) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
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
void RenderingSystem::BindSampledImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, OpaqueHandle image) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
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
void RenderingSystem::BindSamplerToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, SamplerHandle sampler) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
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
*	Binds a storage image to a render data table.
*/
void RenderingSystem::BindStorageImageToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, OpaqueHandle image) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
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
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a storage buffer to a render data table.
*/
void RenderingSystem::BindStorageBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, BufferHandle buffer) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanBuffer *const RESTRICT vulkanBuffer{ static_cast<VulkanBuffer *const RESTRICT>(buffer) };

	//Create the destriptor buffer info.
	VkDescriptorBufferInfo descriptorBufferInfo;

	descriptorBufferInfo.buffer = vulkanBuffer->Get();
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = VK_WHOLE_SIZE;

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = arrayElement;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	writeDescriptorSet.pImageInfo = nullptr;
	writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
	writeDescriptorSet.pTexelBufferView = nullptr;

	//Update the descriptor set.
	vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &writeDescriptorSet, 0, nullptr);
}

/*
*	Binds a uniform buffer to a render data table.
*/
void RenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 arrayElement, RenderDataTableHandle *const RESTRICT handle, BufferHandle buffer) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanBuffer *const RESTRICT vulkanBuffer{ static_cast<VulkanBuffer *const RESTRICT>(buffer) };

	//Create the destriptor buffer info.
	VkDescriptorBufferInfo descriptorBufferInfo;

	descriptorBufferInfo.buffer = vulkanBuffer->Get();
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = VK_WHOLE_SIZE;

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
*	Destroys a render data table.
*/
void RenderingSystem::DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	VulkanRenderingSystemData::_DestructionQueue.Emplace(VulkanRenderingSystemData::VulkanDestructionData::Type::RENDER_DATA_TABLE, *handle);
}

/*
*	Creates a render target.
*/
void RenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render target.
	*handle = static_cast<RenderTargetHandle>(VulkanInterface::Instance->CreateRenderTarget(VulkanTranslationUtilities::GetVulkanExtent(resolution),
																							VulkanTranslationUtilities::GetVulkanFormat(format)));
}

/*
*	Creates a sampler.
*/
void RenderingSystem::CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT
{
	*handle = VulkanInterface::Instance->CreateSampler(	VulkanTranslationUtilities::GetVulkanTextureFilter(properties._MagnificationFilter),
														VulkanTranslationUtilities::GetVulkanMipmapMode(properties._MipmapMode),
														VulkanTranslationUtilities::GetVulkanAddressMode(properties._AddressMode));
}

/*
*	Creates a texture 2D.
*/
void RenderingSystem::CreateTexture2D(const TextureData &data, Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the texture 2D.
	*handle = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(	static_cast<uint32>(data._TextureDataContainer._TextureData.Size()),
																						data._TextureDataContainer._TextureWidth,
																						data._TextureDataContainer._TextureHeight,
																						data._TextureDataContainer._TextureChannels,
																						data._TextureDataContainer._TextureTexelSize,
																						data._TextureDataContainer._TextureData.Data(),
																						VulkanTranslationUtilities::GetVulkanFormat(data._TextureFormat)));
}

/*
*	Creates a texture 3D.
*/
void RenderingSystem::CreateTexture3D(const TextureData& data, Texture3DHandle* const RESTRICT handle) const NOEXCEPT
{
	//Create the texture 3D.
	*handle = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create3DTexture(	static_cast<uint32>(data._TextureDataContainer._TextureData.Size()),
																						data._TextureDataContainer._TextureWidth,
																						data._TextureDataContainer._TextureHeight,
																						data._TextureDataContainer._TextureDepth,
																						data._TextureDataContainer._TextureChannels,
																						data._TextureDataContainer._TextureTexelSize,
																						data._TextureDataContainer._TextureData.Data(),
																						VulkanTranslationUtilities::GetVulkanFormat(data._TextureFormat)));
}

/*
*	Destroys a texture 2D.
*/
void RenderingSystem::DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	VulkanRenderingSystemData::_DestructionQueue.Emplace(VulkanRenderingSystemData::VulkanDestructionData::Type::TEXTURE_2D, *handle);
}

/*
*	Creates a texture cube.
*/
void RenderingSystem::CreateTextureCube(const TextureCubeData& data, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the texture cube.
	*handle = static_cast<TextureCubeHandle>(VulkanInterface::Instance->CreateCubeMapTexture(data._Data.Data(), data._Resolution, data._Resolution));
}

/*
*	Initializes a pipeline
*/
void RenderingSystem::InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT
{
	switch (pipeline->GetType())
	{
		case Pipeline::Type::Compute:
		{
			VulkanRenderingSystemLogic::InitializeComputePipeline(static_cast<ComputePipeline *const RESTRICT>(pipeline));

			break;
		}

		case Pipeline::Type::Graphics:
		{
			VulkanRenderingSystemLogic::InitializeGraphicsPipeline(static_cast<GraphicsPipeline *const RESTRICT>(pipeline));

			break;
		}

		case Pipeline::Type::RayTracing:
		{
			if (RenderingSystem::Instance->IsRayTracingSupported())
			{
				VulkanRenderingSystemLogic::InitializeRayTracingPipeline(static_cast<RayTracingPipeline *const RESTRICT>(pipeline));
			}
			
			break;
		}
	}

	//Add the command buffers.
	const uint64 number_of_command_buffers{ VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages() };
	pipeline->SetNumberOfCommandBuffers(number_of_command_buffers);

	//Create the pipeline command pool.
	CommandPoolHandle pipeline_command_pool;
	CreateCommandPool(pipeline->GetType(), CommandPoolCreateFlags::RESET_COMMAND_BUFFER, &pipeline_command_pool);

	for (uint64 i = 0; i < number_of_command_buffers; ++i)
	{
		CATALYST_BENCHMARK_AVERAGE_SECTION_START();
		pipeline->AddCommandBuffer(AllocateCommandBuffer(pipeline_command_pool, CommandBufferLevel::SECONDARY));
		CATALYST_BENCHMARK_AVERAGE_SECTION_END("hrrm");
	}
}

/*
*	Pre-initializes the rendering system.
*/
void RenderingSystem::PreInitialize() NOEXCEPT
{
	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize();

	//Initialize the Vulkan frame data.
	VulkanRenderingSystemData::_FrameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapChainImages());
}

/*
*	Begins a frame.
*/
void RenderingSystem::BeginFrame() NOEXCEPT
{
	//Pre-update the Vulkan interface.
	VulkanInterface::Instance->PreUpdate(VulkanRenderingSystemData::_FrameData.GetImageAvailableSemaphore());

	//Process the destruction queue.
	VulkanRenderingSystemLogic::ProcessDestructionQueue();

	//Set the current frame.
	VulkanRenderingSystemData::_FrameData.SetCurrentFrame(VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());

	//Wait for the current fence to finish.
	VulkanRenderingSystemData::_FrameData.GetCurrentFence()->WaitFor();

	//Reset the current fence.
	VulkanRenderingSystemData::_FrameData.GetCurrentFence()->Reset();
}

/*
*	Ends a frame.
*/
void RenderingSystem::EndFrame() NOEXCEPT
{
	//Concatenate all secondary command buffers into the previous one.
	VulkanRenderingSystemLogic::ConcatenateCommandBuffers();

	//End the current command buffer.
	VulkanRenderingSystemData::_FrameData.GetCurrentPrimaryCommandBuffer()->End();

	//Submit current command buffer.
	VulkanInterface::Instance->GetGraphicsQueue()->Submit(	*VulkanRenderingSystemData::_FrameData.GetCurrentPrimaryCommandBuffer(),
															1,
															VulkanRenderingSystemData::_FrameData.GetImageAvailableSemaphore(),
															VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
															1,
															VulkanRenderingSystemData::_FrameData.GetRenderFinishedSemaphore(),
															VulkanRenderingSystemData::_FrameData.GetCurrentFence()->Get());

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(VulkanRenderingSystemData::_FrameData.GetRenderFinishedSemaphore());
}
#endif