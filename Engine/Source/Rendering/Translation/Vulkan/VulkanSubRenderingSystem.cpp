#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Translation/Vulkan/VulkanSubRenderingSystem.h>

//Concurrency.
#include <Concurrency/AssertLock.h>
#include <Concurrency/ScopedLock.h>

//File.
#include <File/Core/File.h>
#include <File/Writers/PNGWriter.h>
#include <File/Writers/TGAWriter.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Math.
#include <Math/Core/BaseMath.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanPlatform.h>
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/Vertex.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Rendering/Native/RenderPasses/ImGuiRenderPass.h>
#endif
#include <Rendering/Translation/Vulkan/VulkanFrameData.h>
#include <Rendering/Translation/Vulkan/VulkanComputePipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanGraphicsPipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanRayTracingPipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

//Third party.
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <ThirdParty/ImGui/imgui.h>
#include <ThirdParty/ImGui/imgui_impl_vulkan.h>
#endif

//Vulkan rendering system data.
namespace VulkanSubRenderingSystemData
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
			DEPTH_BUFFER,
			RENDER_DATA_TABLE,
			RENDER_DATA_TABLE_LAYOUT,
			RENDER_TARGET,
			TEXTURE_2D
		};

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

	//The destruction queues.
	DynamicArray<DynamicArray<VulkanDestructionData>> _DestructionQueues;

	//The destruction queue lock.
	AssertLock _DestructionQueueLock;

	//The async destruction task.
	Task _AsycnDestructionTask;

	//The async destruction queue.
	DynamicArray<VulkanDestructionData> _AsyncDestructionQueue;

}

//Vulkan rendering system logic.
namespace VulkanSubRenderingSystemLogic
{

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

		parameters._ShaderModule = static_cast<VulkanShaderModule *const RESTRICT>(pipeline->GetShader());

		//Create the pipeline sub stage data.
		VulkanComputePipelineData *const RESTRICT data{ new (MemorySystem::Instance->TypeAllocate<VulkanComputePipelineData>()) VulkanComputePipelineData() };
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
		VulkanGraphicsPipelineData *const RESTRICT data{ new (MemorySystem::Instance->TypeAllocate<VulkanGraphicsPipelineData>()) VulkanGraphicsPipelineData() };
		pipeline->SetData(data);

		{
			//Create the Vulkan render pass.
			VulkanRenderPassCreationParameters parameters;

			//Determine the attachments that will be used by the pipelines in this render pass.
			Map<VulkanRenderTarget *RESTRICT, uint32> uniqueAttachments;
			Map<VulkanRenderTarget *RESTRICT, uint32> resolve_attachments;
			uint32 attachmentCounter{ 0 };

			for (const RenderTargetHandle output_render_target : pipeline->GetOutputRenderTargets())
			{
				VulkanRenderTarget* const RESTRICT vulkan_render_target{ static_cast<VulkanRenderTarget *const RESTRICT>(output_render_target) };

				if (!uniqueAttachments.Find(vulkan_render_target))
				{
					uniqueAttachments.Emplace(vulkan_render_target, attachmentCounter++);
				}
			}

			for (const RenderTargetHandle output_render_target : pipeline->GetOutputRenderTargets())
			{
				VulkanRenderTarget* const RESTRICT vulkan_render_target{ static_cast<VulkanRenderTarget *const RESTRICT>(output_render_target) };

				if (vulkan_render_target->GetSampleCount() > VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT
					&& !resolve_attachments.Find(vulkan_render_target))
				{
					resolve_attachments.Emplace(vulkan_render_target, attachmentCounter++);
				}
			}

			//Cache the depth buffer.
			VulkanDepthBuffer *const RESTRICT depth_buffer{ static_cast<VulkanDepthBuffer *const RESTRICT>(pipeline->GetDepthBuffer()) };

			//Create the attachment descriptions.
			DynamicArray<VkAttachmentDescription> attachmentDescriptions;
			attachmentDescriptions.Reserve(uniqueAttachments.Size() + resolve_attachments.Size() + (depth_buffer ? 1 : 0));

			DynamicArray<VkAttachmentReference> colorAttachmentReferences;
			colorAttachmentReferences.Reserve(uniqueAttachments.Size());

			VkAttachmentReference depthStencilAttachmentReference;

			uint32 counter{ 0 };

			const bool pipeline_should_clear{ pipeline->GetDepthStencilAttachmentLoadOperator() == AttachmentLoadOperator::CLEAR };

			if (depth_buffer)
			{
				attachmentDescriptions.Emplace(VulkanUtilities::CreateAttachmentDescription(depth_buffer->GetFormat(),
																							depth_buffer->GetSampleCount(),
																							VulkanTranslationUtilities::GetVulkanAttachmentLoadOperator(pipeline->GetDepthStencilAttachmentLoadOperator()),
																							VulkanTranslationUtilities::GetVulkanAttachmentStoreOperator(pipeline->GetDepthStencilAttachmentStoreOperator()),
																							VulkanTranslationUtilities::GetVulkanAttachmentLoadOperator(pipeline->GetDepthStencilAttachmentLoadOperator()),
																							VulkanTranslationUtilities::GetVulkanAttachmentStoreOperator(pipeline->GetDepthStencilAttachmentStoreOperator()),
																							VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
																							VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));

				depthStencilAttachmentReference = VkAttachmentReference{ counter++, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
			}

			for (const Pair<VulkanRenderTarget *RESTRICT, uint32> uniqueAttachment : uniqueAttachments)
			{
				attachmentDescriptions.Emplace(VulkanUtilities::CreateAttachmentDescription(uniqueAttachment._First->GetFormat(),
																							uniqueAttachment._First->GetSampleCount(),
																							VulkanTranslationUtilities::GetVulkanAttachmentLoadOperator(pipeline->GetColorAttachmentLoadOperator()),
																							VulkanTranslationUtilities::GetVulkanAttachmentStoreOperator(pipeline->GetColorAttachmentStoreOperator()),
																							VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																							VK_ATTACHMENT_STORE_OP_DONT_CARE,
																							VK_IMAGE_LAYOUT_GENERAL,
																							VK_IMAGE_LAYOUT_GENERAL));

				colorAttachmentReferences.Emplace(VkAttachmentReference{ counter++, VK_IMAGE_LAYOUT_GENERAL });
			}

			DynamicArray<VkAttachmentReference> resolve_attachment_references;

			for (const Pair<VulkanRenderTarget *RESTRICT, uint32> resolve_attachment : resolve_attachments)
			{
				attachmentDescriptions.Emplace(VulkanUtilities::CreateAttachmentDescription(resolve_attachment._First->GetFormat(),
																							VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
																							VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																							VK_ATTACHMENT_STORE_OP_STORE,
																							VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																							VK_ATTACHMENT_STORE_OP_DONT_CARE,
																							VK_IMAGE_LAYOUT_GENERAL,
																							VK_IMAGE_LAYOUT_GENERAL));

				resolve_attachment_references.Emplace(VkAttachmentReference{ counter++, VK_IMAGE_LAYOUT_GENERAL });
			}

			if (pipeline->IsRenderingDirectlyToScreen())
			{
				attachmentDescriptions.Emplace(VulkanUtilities::CreateAttachmentDescription(VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format,
																							VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
																							VulkanTranslationUtilities::GetVulkanAttachmentLoadOperator(pipeline->GetColorAttachmentLoadOperator()),
																							VulkanTranslationUtilities::GetVulkanAttachmentStoreOperator(pipeline->GetColorAttachmentStoreOperator()),
																							VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																							VK_ATTACHMENT_STORE_OP_DONT_CARE,
																							VK_IMAGE_LAYOUT_UNDEFINED,
																							VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));

				colorAttachmentReferences.Emplace(VkAttachmentReference{ counter++, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
			}

			parameters._AttachmentCount = static_cast<uint32>(attachmentDescriptions.Size());
			parameters._AttachmentDescriptions = attachmentDescriptions.Data();

			//Create the subpass descriptions.
			VkSubpassDescription subpassDescription{ VulkanUtilities::CreateSubpassDescription(	0,
																								nullptr,
																								static_cast<uint32>(colorAttachmentReferences.Size()),
																								colorAttachmentReferences.Data(),
																								resolve_attachment_references.Empty() ? nullptr : resolve_attachment_references.Data(),
																								depth_buffer ? &depthStencilAttachmentReference : nullptr,
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

			//Set whether or not multiview should be enabled.
			parameters._MultiviewEnabled = pipeline->GetMultiviewEnabled();

			//Create the render pass!
			data->_RenderPass = VulkanInterface::Instance->CreateRenderPass(parameters);

			//Create the framebuffer(s).
			if (pipeline->IsRenderingDirectlyToScreen())
			{
				//Create the framebuffers.
				const DynamicArray<VkImageView> &swapchainImages{ VulkanInterface::Instance->GetSwapchain().GetSwapchainImageViews() };
				data->_FrameBuffers.Reserve(swapchainImages.Size());

				for (VkImageView swapchainImage : swapchainImages)
				{
					VulkanFramebufferCreationParameters framebufferParameters;

					framebufferParameters._RenderPass = data->_RenderPass->Get();

					DynamicArray<VkImageView> attachments;
					attachments.Reserve(1 + (depth_buffer ? 1 : 0));

					if (depth_buffer)
					{
						attachments.Emplace(depth_buffer->GetImageView());
					}

					attachments.Emplace(swapchainImage);

					framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
					framebufferParameters._Attachments = attachments.Data();
					framebufferParameters._Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
					framebufferParameters._NumberOfLayers = 2;
#else
					framebufferParameters._NumberOfLayers = 1;
#endif

					data->_FrameBuffers.Emplace(VulkanInterface::Instance->CreateFramebuffer(framebufferParameters));
				}

				data->_Extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
				data->_NumberOfAttachments = static_cast<uint32>(1 + (depth_buffer ? 1 : 0));
				data->_RenderToScreeen = true;
			}

			else
			{
				VulkanFramebufferCreationParameters framebufferParameters;

				framebufferParameters._RenderPass = data->_RenderPass->Get();

				DynamicArray<VkImageView> attachments;
				attachments.Reserve(uniqueAttachments.Size() + resolve_attachments.Size() + (depth_buffer ? 1 : 0));

				if (depth_buffer)
				{
					attachments.Emplace(depth_buffer->GetImageView());
				}

				for (const Pair<VulkanRenderTarget *RESTRICT, uint32> uniqueAttachment : uniqueAttachments)
				{
					attachments.Emplace(static_cast<VulkanRenderTarget *const RESTRICT>(uniqueAttachment._First)->GetImageView());
				}

				for (const Pair<VulkanRenderTarget *RESTRICT, uint32> resolve_attachment : resolve_attachments)
				{
					attachments.Emplace(static_cast<VulkanRenderTarget *const RESTRICT>(resolve_attachment._First)->GetResolveImageView());
				}

				framebufferParameters._AttachmentCount = static_cast<uint32>(attachments.Size());
				framebufferParameters._Attachments = attachments.Data();
				framebufferParameters._Extent = { pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height };
				framebufferParameters._NumberOfLayers = 1;

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
			parameters._ColorBlendOperator = VulkanTranslationUtilities::GetVulkanBlendOperator(pipeline->GetColorBlendOperator());
			parameters._BlendFactorSourceAlpha = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorSourceAlpha());
			parameters._BlendFactorDestinationAlpha = VulkanTranslationUtilities::GetVulkanBlendFactor(pipeline->GetBlendFactorDestinationAlpha());
			parameters._AlphaBlendOperator = VulkanTranslationUtilities::GetVulkanBlendOperator(pipeline->GetAlphaBlendOperator());
			parameters._ColorAttachmentCount = static_cast<uint32>(pipeline->GetOutputRenderTargets().Size()) + static_cast<uint32>(pipeline->IsRenderingDirectlyToScreen());
			parameters._CullMode = VulkanTranslationUtilities::GetVulkanCullMode(pipeline->GetCullMode());
			
			{
				parameters._SampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

				if (pipeline->GetDepthBuffer())
				{
					parameters._SampleCount = BaseMath::Maximum(parameters._SampleCount, static_cast<VulkanDepthBuffer* const RESTRICT>(pipeline->GetDepthBuffer())->GetSampleCount());
				}

				for (const RenderTargetHandle output_render_target : pipeline->GetOutputRenderTargets())
				{
					parameters._SampleCount = BaseMath::Maximum(parameters._SampleCount, static_cast<VulkanRenderTarget *const RESTRICT>(output_render_target)->GetSampleCount());
				}

#if !defined(CATALYST_CONFIGURATION_FINAL)
				if (pipeline->GetDepthBuffer())
				{
					ASSERT(parameters._SampleCount == static_cast<VulkanDepthBuffer* const RESTRICT>(pipeline->GetDepthBuffer())->GetSampleCount(), "Sample count doesn't match!");
				}

				for (const RenderTargetHandle output_render_target : pipeline->GetOutputRenderTargets())
				{
					ASSERT(parameters._SampleCount == static_cast<VulkanRenderTarget *const RESTRICT>(output_render_target)->GetSampleCount(), "Sample count doesn't match!");
				}
#endif
			}
			
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
			if (pipeline->GetVertexShader() != EMPTY_HANDLE) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetVertexShader()));
			if (pipeline->GetTessellationControlShader() != EMPTY_HANDLE) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetTessellationControlShader()));
			if (pipeline->GetTessellationEvaluationShader() != EMPTY_HANDLE) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetTessellationEvaluationShader()));
			if (pipeline->GetGeometryShader() != EMPTY_HANDLE) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetGeometryShader()));
			if (pipeline->GetFragmentShader() != EMPTY_HANDLE) parameters._ShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(pipeline->GetFragmentShader()));
			
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

			if (pipeline->IsRenderingDirectlyToScreen())
			{
				parameters._ViewportExtent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
			}

			else
			{
				parameters._ViewportExtent = pipeline->GetOutputRenderTargets().Empty() ? VkExtent2D{ pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height } : VkExtent2D{ pipeline->GetRenderResolution()._Width, pipeline->GetRenderResolution()._Height };
			}

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

		parameters._RayGenerationShaderModule = static_cast<VulkanShaderModule *const RESTRICT>(pipeline->GetRayGenerationShader());

		for (const RayTracingPipeline::HitGroup hit_group : pipeline->GetHitGroups())
		{
			parameters._HitGroups.Emplace(	hit_group._ClosestHitShader ? static_cast<const VulkanShaderModule *const RESTRICT>(hit_group._ClosestHitShader) : nullptr,
											hit_group._AnyHitShader ? static_cast<const VulkanShaderModule *const RESTRICT>(hit_group._AnyHitShader) : nullptr,
											hit_group._IntersectionShader ? static_cast<const VulkanShaderModule *const RESTRICT>(hit_group._IntersectionShader) : nullptr);
		}

		for (const ShaderHandle shader : pipeline->GetMissShaders())
		{
			parameters._MissShaderModules.Emplace(static_cast<const VulkanShaderModule *const RESTRICT>(shader));
		}

		//Create the pipeline sub stage data.
		VulkanRayTracingPipelineData *const RESTRICT data{ new (MemorySystem::Instance->TypeAllocate<VulkanRayTracingPipelineData>()) VulkanRayTracingPipelineData() };
		pipeline->SetData(data);

		//Create the pipeline!
		data->_Pipeline = VulkanInterface::Instance->CreateRayTracingPipeline(parameters);

		//Create the shader binding table buffer.
		const uint32 shader_group_handle_size{ VulkanInterface::Instance->GetPhysicalDevice().GetRayTracingProperties().shaderGroupHandleSize };
		const uint32 shader_group_base_alignment{ VulkanInterface::Instance->GetPhysicalDevice().GetRayTracingProperties().shaderGroupBaseAlignment };
		const uint32 shader_group_handle_size_aligned{ BaseMath::RoundToNearestMultipleOf(shader_group_handle_size, shader_group_base_alignment) };

		//Calculate the shader binding table size.
		uint32 number_of_handles{ 0 };

		//Calculate the ray generation shader group.
		const uint32 ray_generation_size{ shader_group_handle_size_aligned };
		++number_of_handles;

		//Calculate the hit shader groups.
		const uint32 hit_shader_groups_size{ BaseMath::RoundToNearestMultipleOf(static_cast<uint32>(shader_group_handle_size * pipeline->GetHitGroups().Size()), shader_group_base_alignment) };
		number_of_handles += static_cast<uint32>(pipeline->GetHitGroups().Size());

		//Calculate the miss shadergroups.
		const uint32 miss_shader_groups_size{ BaseMath::RoundToNearestMultipleOf(static_cast<uint32>(shader_group_handle_size * pipeline->GetMissShaders().Size()), shader_group_base_alignment) };
		number_of_handles += static_cast<uint32>(pipeline->GetMissShaders().Size());

		//Calculate the handles data size.
		const uint32 handles_data_size{ shader_group_handle_size * number_of_handles };

		//Retrieve the handles
		uint8 *const RESTRICT handles_storage{ static_cast<uint8* const RESTRICT>(Memory::Allocate(handles_data_size)) };

		VULKAN_ERROR_CHECK
		(
			vkGetRayTracingShaderGroupHandlesNV
			(
				VulkanInterface::Instance->GetLogicalDevice().Get(),
				data->_Pipeline->GetPipeline(),
				0,
				number_of_handles,
				handles_data_size,
				handles_storage
			)
		);

		//Calculate the shader binding table size.
		const uint32 shader_binding_table_size{ ray_generation_size + hit_shader_groups_size + miss_shader_groups_size };

		//Allocate data for the shader binding table handles.
		uint8 *const RESTRICT shader_binding_table_handles_storage{ static_cast<uint8* const RESTRICT>(Memory::Allocate(shader_binding_table_size)) };

		//Copy data into it.
		uint64 current_offset{ 0 };
		uint64 handle_index{ 0 };

		Memory::Copy(&shader_binding_table_handles_storage[current_offset], &handles_storage[shader_group_handle_size * handle_index++], shader_group_handle_size);

		current_offset = BaseMath::RoundToNearestMultipleOf(static_cast<uint32>(current_offset + shader_group_handle_size), shader_group_base_alignment);

		for (uint64 i{ 0 }; i < pipeline->GetHitGroups().Size(); ++i)
		{
			Memory::Copy(&shader_binding_table_handles_storage[current_offset], &handles_storage[shader_group_handle_size * handle_index++], shader_group_handle_size);
			current_offset += shader_group_handle_size;
		}

		current_offset = BaseMath::RoundToNearestMultipleOf(static_cast<uint32>(current_offset), shader_group_base_alignment);

		for (uint64 i{ 0 }; i < pipeline->GetMissShaders().Size(); ++i)
		{
			Memory::Copy(&shader_binding_table_handles_storage[current_offset], &handles_storage[shader_group_handle_size * handle_index++], shader_group_handle_size);
			current_offset += shader_group_handle_size;
		}

		current_offset = BaseMath::RoundToNearestMultipleOf(static_cast<uint32>(current_offset), shader_group_base_alignment);

		//Create the shader binding table buffer.
		data->_ShaderBindingTableBuffer = VulkanInterface::Instance->CreateBuffer(shader_binding_table_size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		void *const RESTRICT data_chunks[]{ shader_binding_table_handles_storage };
		const uint64 data_sizes[]{ shader_binding_table_size };

		data->_ShaderBindingTableBuffer->UploadData(data_chunks, data_sizes, 1);

		Memory::Free(handles_storage);
		Memory::Free(shader_binding_table_handles_storage);

		//Calculate the offsets/strides.
		data->_HitShaderBindingOffset = ray_generation_size;
		data->_HitShaderBindingStride = shader_group_handle_size;
		data->_MissShaderBindingOffset = ray_generation_size + hit_shader_groups_size;
		data->_MissShaderBindingStride = shader_group_handle_size;
	}

	/*
	*	Terminates a compute pipeline.
	*/
	void TerminateComputePipeline(ComputePipeline *const RESTRICT pipeline) NOEXCEPT
	{
		//Cache the data.
		VulkanComputePipelineData *const RESTRICT data{ static_cast<VulkanComputePipelineData *const RESTRICT>(pipeline->GetData()) };

		//Destroy the pipeline
		VulkanInterface::Instance->DestroyComputePipeline(data->_Pipeline);

		//Free the data.
		data->~VulkanComputePipelineData();
		MemorySystem::Instance->TypeFree<VulkanComputePipelineData>(data);
	}

	/*
	*	Terminates a graphics pipeline.
	*/
	void TerminateGraphicsPipeline(GraphicsPipeline *const RESTRICT pipeline) NOEXCEPT
	{
		//Cache the data.
		VulkanGraphicsPipelineData *const RESTRICT data{ static_cast<VulkanGraphicsPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Destroy the framebuffers.
		for (VulkanFramebuffer *const RESTRICT framebuffer : data->_FrameBuffers)
		{
			VulkanInterface::Instance->DestroyFramebuffer(framebuffer);
		}

		//Destroy the render pass.
		VulkanInterface::Instance->DestroyRenderPass(data->_RenderPass);

		//Destroy the pipeline.
		VulkanInterface::Instance->DestroyGraphicsPipeline(data->_Pipeline);

		//Free the data.
		data->~VulkanGraphicsPipelineData();
		MemorySystem::Instance->TypeFree<VulkanGraphicsPipelineData>(data);
	}

	/*
	*	Terminates a ray tracing pipeline.
	*/
	void TerminateRayTracingPipeline(RayTracingPipeline *const RESTRICT pipeline) NOEXCEPT
	{
		//Cache the data.
		VulkanRayTracingPipelineData *const RESTRICT data{ static_cast<VulkanRayTracingPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Destroy the shader binding table buffer.
		VulkanInterface::Instance->DestroyBuffer(data->_ShaderBindingTableBuffer);

		//Destroy the pipeline
		VulkanInterface::Instance->DestroyRayTracingPipeline(data->_Pipeline);

		//Free the data.
		data->~VulkanRayTracingPipelineData();
		MemorySystem::Instance->TypeFree<VulkanRayTracingPipelineData>(data);
	}

	/*
	*	Processes the destruction queue.
	*/
	void ProcessDestructionQueue(const uint8 framebuffer_index) NOEXCEPT
	{
		SCOPED_LOCK(VulkanSubRenderingSystemData::_DestructionQueueLock);

		//Cache the current destruction queue.
		DynamicArray<VulkanSubRenderingSystemData::VulkanDestructionData> &current_destruction_queue{ VulkanSubRenderingSystemData::_DestructionQueues[framebuffer_index] };

		//Is the async destruction task ready?
		if (VulkanSubRenderingSystemData::_AsycnDestructionTask.IsExecuted())
		{
			//Check if any destruction data should be added to the async destruction queue.
			for (uint64 i{ 0 }; i < current_destruction_queue.Size();)
			{
				VulkanSubRenderingSystemData::_AsyncDestructionQueue.Emplace(current_destruction_queue[i]);
				current_destruction_queue.EraseAt<false>(i);
			}

			current_destruction_queue.Clear();

			if (!VulkanSubRenderingSystemData::_AsyncDestructionQueue.Empty())
			{
				TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, &VulkanSubRenderingSystemData::_AsycnDestructionTask);
			}
		}
	}

	/*
	*	Processes the destruction queue asynchrounously.
	*/
	void ProcessDestructionQueueAsynchronous() NOEXCEPT
	{
		for (uint64 i{ 0 }; i < VulkanSubRenderingSystemData::_AsyncDestructionQueue.Size(); ++i)
		{
			switch (VulkanSubRenderingSystemData::_AsyncDestructionQueue[i]._Type)
			{
				case VulkanSubRenderingSystemData::VulkanDestructionData::Type::ACCELERATION_STRUCTURE:
				{
					VulkanInterface::Instance->DestroyAccelerationStructure(static_cast<VulkanAccelerationStructure* const RESTRICT>(VulkanSubRenderingSystemData::_AsyncDestructionQueue[i]._Handle));

					break;
				}

				case VulkanSubRenderingSystemData::VulkanDestructionData::Type::BUFFER:
				{
					VulkanInterface::Instance->DestroyBuffer(static_cast<VulkanBuffer* const RESTRICT>(VulkanSubRenderingSystemData::_AsyncDestructionQueue[i]._Handle));

					break;
				}

				case VulkanSubRenderingSystemData::VulkanDestructionData::Type::DEPTH_BUFFER:
				{
					VulkanInterface::Instance->DestroyDepthBuffer(static_cast<VulkanDepthBuffer* const RESTRICT>(VulkanSubRenderingSystemData::_AsyncDestructionQueue[i]._Handle));

					break;
				}

				case VulkanSubRenderingSystemData::VulkanDestructionData::Type::RENDER_DATA_TABLE:
				{
					VulkanInterface::Instance->DestroyDescriptorSet(static_cast<VulkanDescriptorSet* const RESTRICT>(VulkanSubRenderingSystemData::_AsyncDestructionQueue[i]._Handle));

					break;
				}

				case VulkanSubRenderingSystemData::VulkanDestructionData::Type::RENDER_DATA_TABLE_LAYOUT:
				{
					VulkanInterface::Instance->DestroyDescriptorSetLayout(static_cast<VulkanDescriptorSetLayout* const RESTRICT>(VulkanSubRenderingSystemData::_AsyncDestructionQueue[i]._Handle));

					break;
				}

				case VulkanSubRenderingSystemData::VulkanDestructionData::Type::RENDER_TARGET:
				{
					VulkanInterface::Instance->DestroyRenderTarget(static_cast<VulkanRenderTarget* const RESTRICT>(VulkanSubRenderingSystemData::_AsyncDestructionQueue[i]._Handle));

					break;
				}

				case VulkanSubRenderingSystemData::VulkanDestructionData::Type::TEXTURE_2D:
				{
					VulkanInterface::Instance->Destroy2DTexture(static_cast<Vulkan2DTexture* const RESTRICT>(VulkanSubRenderingSystemData::_AsyncDestructionQueue[i]._Handle));

					break;
				}

				default:
				{
					ASSERT(false, "Unhandled case!");

					break;
				}
			}
		}

		VulkanSubRenderingSystemData::_AsyncDestructionQueue.Clear();
	}

}

/*
*	Pre-initializes the sub rendering system.
*/
void VulkanSubRenderingSystem::PreInitialize() NOEXCEPT
{
	//Initialize the Vulkan interface.
	VulkanInterface::Instance->Initialize();

	//Initialize the Vulkan frame data.
	VulkanSubRenderingSystemData::_FrameData.Initialize(VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapchainImages());

	//Set up the destruction queues.
	VulkanSubRenderingSystemData::_DestructionQueues.Upsize<true>(GetNumberOfFramebuffers());
}

/*
*	Initializes the sub rendering system.
*/
void VulkanSubRenderingSystem::Initialize() NOEXCEPT
{
	//Set up the async destruction task.
	VulkanSubRenderingSystemData::_AsycnDestructionTask._Function = [](void *const RESTRICT)
	{
		VulkanSubRenderingSystemLogic::ProcessDestructionQueueAsynchronous();
	};
	VulkanSubRenderingSystemData::_AsycnDestructionTask._Arguments = nullptr;
	VulkanSubRenderingSystemData::_AsycnDestructionTask._ExecutableOnSameThread = false;
}

/*
*	Post-initializes the sub rendering system.
*/
void VulkanSubRenderingSystem::PostInitialize() NOEXCEPT
{
#if !defined(CATALYST_CONFIGURATION_FINAL)
	if (RenderingSystem::Instance->GetCurrentRenderingPath() != RenderingPath::CUSTOM)
	{
		//Set up ImGui for Vulkan.
		{
			ImGui_ImplVulkan_InitInfo imgui_init_info{ };

			imgui_init_info.Instance = VulkanInterface::Instance->GetInstance().Get();
			imgui_init_info.PhysicalDevice = VulkanInterface::Instance->GetPhysicalDevice().Get();
			imgui_init_info.Device = VulkanInterface::Instance->GetLogicalDevice().Get();
			imgui_init_info.QueueFamily = VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::MAIN);
			imgui_init_info.Queue = VulkanInterface::Instance->GetMainQueue()->Get();
			imgui_init_info.DescriptorPool = VulkanInterface::Instance->GetDescriptorPool().Get();
			imgui_init_info.RenderPass = static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(ImGuiRenderPass::Instance->GetImGuiGraphicsPipeline()->GetData())->_RenderPass->Get();
			imgui_init_info.MinImageCount = VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapchainImages();
			imgui_init_info.ImageCount = VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapchainImages();
			imgui_init_info.MSAASamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

			imgui_init_info.UseDynamicRendering = false;

			imgui_init_info.Allocator = nullptr;
			imgui_init_info.CheckVkResultFn = [](VkResult result)
			{
				ASSERT(result == VkResult::VK_SUCCESS, "Oh no!");
			};
			imgui_init_info.MinAllocationSize = 1024 * 1024;

			ImGui_ImplVulkan_Init(&imgui_init_info);
		}

		//Create the fonts texture.
		ImGui_ImplVulkan_CreateFontsTexture();
	}
#endif
}

/*
*	Terminates the rendering system.
*/
void VulkanSubRenderingSystem::Terminate() NOEXCEPT
{
	//First of all, wait for all queues to finish their tasks.
	VulkanInterface::Instance->GetMainQueue()->WaitIdle();
	VulkanInterface::Instance->GetAsyncComputeQueue()->WaitIdle();
	VulkanInterface::Instance->GetAsyncTransferQueue()->WaitIdle();

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Shut down ImGui.
	if (RenderingSystem::Instance->GetCurrentRenderingPath() != RenderingPath::CUSTOM)
	{
		ImGui_ImplVulkan_Shutdown();
	}
#endif

	//Release the Vulkan interface.
	VulkanInterface::Instance->Release();
}

/*
*	Returns whether or not this sub rendering system can render.
*/
NO_DISCARD bool VulkanSubRenderingSystem::CanRender() const NOEXCEPT
{
	return	VulkanInterface::Instance->GetSurface().Get() != VK_NULL_HANDLE
			&& VulkanInterface::Instance->GetSwapchain().Get() != VK_NULL_HANDLE;
}

/*
*	Notifies this sub rendering system that a rendering platform event has occured.
*/
void VulkanSubRenderingSystem::OnRenderingPlatformEvent(const RenderingPlatformEvent rendering_platform_event) NOEXCEPT
{
	//Was the surface lost?
	if (TEST_BIT(UNDERLYING(rendering_platform_event), UNDERLYING(RenderingPlatformEvent::SURFACE_LOST)))
	{
		//Release the swapchain.
		VulkanInterface::Instance->GetSwapchain().Release();

		//Release the surface.
		VulkanInterface::Instance->GetSurface().Release();
	}

	//Was the surface gained?
	else if (TEST_BIT(UNDERLYING(rendering_platform_event), UNDERLYING(RenderingPlatformEvent::SURFACE_GAINED)))
	{
		//Initialize the surface.
		VulkanInterface::Instance->GetSurface().Initialize();

		//Initialize the swapchain.
		VulkanInterface::Instance->GetSwapchain().Initialize();
	}
}

/*
*	Returns whether or not geometry shaders is supported.
*/
NO_DISCARD bool VulkanSubRenderingSystem::IsGeometryShadersSupported() const NOEXCEPT
{
	//Just check if the physical device has multview support.
	return VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceFeatures().geometryShader == VK_TRUE;
}

/*
*	Returns whether or not multiview is supported.
*/
NO_DISCARD bool VulkanSubRenderingSystem::IsMultiviewSupported() const NOEXCEPT
{
	//Just check if the physical device has multview support.
	return VulkanInterface::Instance->GetPhysicalDevice().HasMultiviewSupport();
}

/*
*	Returns whether or not ray tracing is supported.
*/
NO_DISCARD bool VulkanSubRenderingSystem::IsRayTracingSupported() const NOEXCEPT
{
	//Just check if the physical device has ray tracing support.
	return VulkanInterface::Instance->GetPhysicalDevice().HasRayTracingSupport();
}

/*
*	Returns the number of framebuffers.
*/
uint8 VulkanSubRenderingSystem::GetNumberOfFramebuffers() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetNumberOfSwapchainImages();
}

/*
*	Returns the current framebuffer index.
*/
uint8 VulkanSubRenderingSystem::GetCurrentFramebufferIndex() const NOEXCEPT
{
	return VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex();
}

/*
*	Returns the current surface transform.
*/
SurfaceTransform VulkanSubRenderingSystem::GetCurrentSurfaceTransform() const NOEXCEPT
{
	switch (VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceCapabilities().currentTransform)
	{
		case VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR:
		{
			return SurfaceTransform::IDENTITY;
		}

		case VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
		{
			return SurfaceTransform::ROTATE_90;
		}

		case VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
		{
			return SurfaceTransform::ROTATE_270;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return SurfaceTransform::IDENTITY;
		}
	}
}

/*
*	Creates a bottom level acceleration structure.
*/
void VulkanSubRenderingSystem::CreateBottomLevelAccelerationStructure(	const BufferHandle vertex_buffer,
																		const uint32 number_of_vertices,
																		const BufferHandle index_buffer,
																		const uint32 number_of_indices,
																		const BottomLevelAccelerationStructureFlag flags,
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
		geometry.geometry.triangles.vertexData = static_cast<VulkanBuffer *const RESTRICT>(vertex_buffer)->Get();
		geometry.geometry.triangles.vertexOffset = 0;
		geometry.geometry.triangles.vertexCount = number_of_vertices;
		geometry.geometry.triangles.vertexStride = sizeof(Vertex);
		geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		geometry.geometry.triangles.indexData = static_cast<VulkanBuffer *const RESTRICT>(index_buffer)->Get();
		geometry.geometry.triangles.indexOffset = 0;
		geometry.geometry.triangles.indexCount = number_of_indices;
		geometry.geometry.triangles.indexType = VkIndexType::VK_INDEX_TYPE_UINT32;
		geometry.geometry.triangles.transformData = VK_NULL_HANDLE;
		geometry.geometry.triangles.transformOffset = 0;
		geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
		geometry.geometry.aabbs.pNext = nullptr;
		geometry.geometry.aabbs.aabbData = VK_NULL_HANDLE;
		geometry.geometry.aabbs.numAABBs = 0;
		geometry.geometry.aabbs.stride = 0;
		geometry.geometry.aabbs.offset = 0;
		geometry.flags = VulkanTranslationUtilities::GetVulkanGeometryFlags(flags);

		*handle = VulkanInterface::Instance->CreateAccelerationStructure
		(
			VkAccelerationStructureTypeNV::VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV,
			ArrayProxy<VulkanGeometryInstance>(),
			ArrayProxy<VkGeometryNV>(geometry),
			nullptr
		);
	}

	else
	{
		*handle = EMPTY_HANDLE;
	}
}

/*
*	Creates a top level acceleration structure.
*	Can take an optional command buffer if one is created during a frame.
*/
void VulkanSubRenderingSystem::CreateTopLevelAccelerationStructure
(
	const ArrayProxy<TopLevelAccelerationStructureInstanceData> &instance_data,
	AccelerationStructureHandle *const RESTRICT handle,
	CommandBuffer *const RESTRICT command_buffer
) NOEXCEPT
{
	if (IsRayTracingSupported())
	{
		//Create the Vulkan geometry instance structres.
		VulkanGeometryInstance* const RESTRICT geometry_instances{ MemorySystem::Instance->FrameAllocate<VulkanGeometryInstance>(sizeof(VulkanGeometryInstance) * instance_data.Size()) };

		for (uint64 i{ 0 }, size{ instance_data.Size() }; i < size; ++i)
		{
			VulkanTranslationUtilities::GetVulkanGeometryInstance(instance_data[i], &geometry_instances[i]);
		}

		//This might be a recreate, so check for that.
		VulkanAccelerationStructure *const RESTRICT vulkan_acceleration_structure{ static_cast<VulkanAccelerationStructure *const RESTRICT>(*handle) };

		if (vulkan_acceleration_structure)
		{
			vulkan_acceleration_structure->Initialize
			(
				VkAccelerationStructureTypeNV::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV,
				ArrayProxy<VulkanGeometryInstance>(geometry_instances, instance_data.Size()),
				ArrayProxy<VkGeometryNV>(),
				command_buffer ? static_cast<VulkanCommandBuffer* const RESTRICT>(command_buffer->GetCommandBufferData()) : nullptr
			);
		}

		else
		{
			//Create the acceleration structure.
			*handle = VulkanInterface::Instance->CreateAccelerationStructure
			(
				VkAccelerationStructureTypeNV::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV,
				ArrayProxy<VulkanGeometryInstance>(geometry_instances, instance_data.Size()),
				ArrayProxy<VkGeometryNV>(),
				command_buffer ? static_cast<VulkanCommandBuffer *const RESTRICT>(command_buffer->GetCommandBufferData()) : nullptr
			);
		}
	}
	
	else
	{
		*handle = nullptr;
	}
}

/*
*	Destroys an acceleration structure.
*/
void VulkanSubRenderingSystem::DestroyAccelerationStructure(AccelerationStructureHandle *const RESTRICT handle) NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	SCOPED_LOCK(VulkanSubRenderingSystemData::_DestructionQueueLock);

	VulkanSubRenderingSystemData::_DestructionQueues[GetCurrentFramebufferIndex()].Emplace(VulkanSubRenderingSystemData::VulkanDestructionData::Type::ACCELERATION_STRUCTURE, *handle);
}

/*
*	Creates a buffer.
*/
void VulkanSubRenderingSystem::CreateBuffer(const uint64 size, const BufferUsage usage, const MemoryProperty memory_properties, BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the buffer.
	*handle = static_cast<BufferHandle>(VulkanInterface::Instance->CreateBuffer(size, VulkanTranslationUtilities::GetVulkanBufferUsage(usage), VulkanTranslationUtilities::GetVulkanMemoryProperty(memory_properties)));
}

/*
*	Uploads data to a buffer.
*/
void VulkanSubRenderingSystem::UploadDataToBuffer(const void *const RESTRICT *const RESTRICT data, const uint64 *const RESTRICT data_sizes, const uint8 data_chunks, BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Upload data to the buffer.
	static_cast<VulkanBuffer *const RESTRICT>(*handle)->UploadData(data, data_sizes, data_chunks);
}

/*
*	Destroys a buffer.
*/
void VulkanSubRenderingSystem::DestroyBuffer(BufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	SCOPED_LOCK(VulkanSubRenderingSystemData::_DestructionQueueLock);

	VulkanSubRenderingSystemData::_DestructionQueues[GetCurrentFramebufferIndex()].Emplace(VulkanSubRenderingSystemData::VulkanDestructionData::Type::BUFFER, *handle);
}

/*
*	Creates a command pool.
*/
void VulkanSubRenderingSystem::CreateCommandPool(const Pipeline::Type type, CommandPoolHandle *const RESTRICT handle) const NOEXCEPT
{
	switch (type)
	{
		case Pipeline::Type::COMPUTE:
		case Pipeline::Type::RAY_TRACING:
		{
			*handle = VulkanInterface::Instance->CreateMainCommandPool(0);

			break;
		}

		case Pipeline::Type::GRAPHICS:
		{
			*handle = VulkanInterface::Instance->CreateMainCommandPool(0);

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
*	Resets a command pool.
*/
void VulkanSubRenderingSystem::ResetCommandPool(CommandPoolHandle *const RESTRICT handle) NOEXCEPT
{
	static_cast<VulkanCommandPool *const RESTRICT>(*handle)->Reset();
}

/*
*	Allocates a command buffer from the given command pool.
*/
RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT VulkanSubRenderingSystem::AllocateCommandBuffer(const CommandPoolHandle command_pool, const CommandBufferLevel level) const NOEXCEPT
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
*	Submits a command buffer.
*/
void VulkanSubRenderingSystem::SubmitCommandBuffer(const CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	VulkanInterface::Instance->GetMainQueue()->Submit(	*static_cast<VulkanCommandBuffer *const RESTRICT>(command_buffer->GetCommandBufferData()),
														0,
														nullptr,
														0,
														0,
														nullptr,
														VK_NULL_HANDLE);
}

/*
*	Creates a depth buffer.
*/
void VulkanSubRenderingSystem::CreateDepthBuffer(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, DepthBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the depth buffer.
	*handle = static_cast<DepthBufferHandle>(VulkanInterface::Instance->CreateDepthBuffer(	VkExtent2D{ resolution._Width, resolution._Height },
																							VulkanTranslationUtilities::GetVulkanFormat(format),
																							VulkanTranslationUtilities::GetVulkanSampleCount(sample_count)));
}

/*
*	Destroys a depth buffer.
*/
void VulkanSubRenderingSystem::DestroyDepthBuffer(DepthBufferHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	SCOPED_LOCK(VulkanSubRenderingSystemData::_DestructionQueueLock);

	VulkanSubRenderingSystemData::_DestructionQueues[GetCurrentFramebufferIndex()].Emplace(VulkanSubRenderingSystemData::VulkanDestructionData::Type::DEPTH_BUFFER, *handle);
}

/*
*	Creates an event.
*/
void VulkanSubRenderingSystem::CreateEvent(EventHandle *const RESTRICT handle) NOEXCEPT
{
	*handle = static_cast<EventHandle>(VulkanInterface::Instance->CreateEvent());
}

/*
*	Resets an event.
*/
void VulkanSubRenderingSystem::ResetEvent(EventHandle handle) NOEXCEPT
{
	static_cast<VulkanEvent *const RESTRICT>(handle)->Reset();
}

/*
*	Waits for an event.
*/
void VulkanSubRenderingSystem::WaitForEvent(EventHandle handle) NOEXCEPT
{
	static_cast<VulkanEvent *const RESTRICT>(handle)->WaitFor();
}

/*
*	Creates a query pool.
*/
void VulkanSubRenderingSystem::CreateQueryPool(const uint32 query_count, QueryPoolHandle *const RESTRICT handle) NOEXCEPT
{
	*handle = static_cast<QueryPoolHandle>(VulkanInterface::Instance->CreateQueryPool(VkQueryType::VK_QUERY_TYPE_TIMESTAMP, query_count));
}

/*
*	Resets the given query pool.
*/
void VulkanSubRenderingSystem::ResetQueryPool(QueryPoolHandle *const RESTRICT handle) NOEXCEPT
{
	static_cast<VulkanQueryPool *const RESTRICT>(*handle)->Reset();
}

/*
*	Returns the execution time, in nanoseconds, from the given query pool.
*	Assumption being that the query pool has been used to record two timestamps into a command buffer that has completed.
*/
NO_DISCARD uint64 VulkanSubRenderingSystem::GetExecutionTime(const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT
{
	//Get the query results.
	StaticArray<uint64, 2> query_pool_results;
	VULKAN_ERROR_CHECK
	(
		vkGetQueryPoolResults
		(
			VulkanInterface::Instance->GetLogicalDevice().Get(),
			static_cast<const VulkanQueryPool *const RESTRICT>(query_pool)->Get(),
			query_index,
			2,
			sizeof(uint64) * 2,
			query_pool_results.Data(),
			sizeof(uint64),
			VkQueryResultFlagBits::VK_QUERY_RESULT_64_BIT | VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT
		)
	);

	//Cache the timestamp valids bits/timestamp period.
	const uint64 timestamp_valid_bits{ static_cast<uint64>(VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyProperties(VulkanLogicalDevice::QueueType::MAIN).timestampValidBits) };
	const float64 timestamp_period{ static_cast<float64>(VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceProperties().limits.timestampPeriod) };

	return BaseMath::Round<uint64>(static_cast<float32>(static_cast<float64>(query_pool_results[1] - query_pool_results[0]) * timestamp_period));
}

/*
*	Creates a Shader.
*/
void VulkanSubRenderingSystem::CreateShader(const ArrayProxy<byte> &data, const ShaderStage stage, ShaderHandle *const RESTRICT handle) const NOEXCEPT
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
void VulkanSubRenderingSystem::CreateRenderDataTableLayout(const RenderDataTableLayoutBinding *const RESTRICT bindings, const uint32 number_of_bindings, RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	//Translate from the API-agnostic structure to the Vulkan-specific.
	DynamicArray<VkDescriptorSetLayoutBinding> vulkanBindings;
	vulkanBindings.Reserve(number_of_bindings);

	for (uint32 i = 0; i < number_of_bindings; ++i)
	{
		const RenderDataTableLayoutBinding &binding{ bindings[i] };

		vulkanBindings.Emplace(VulkanUtilities::CreateDescriptorSetLayoutBinding(binding._Binding, VulkanTranslationUtilities::GetVulkanDescriptorType(binding._Type), binding._NumberOfArrayElements, VulkanTranslationUtilities::GetVulkanShaderStages(binding._ShaderStage)));
	}

	*handle = VulkanInterface::Instance->CreateDescriptorSetLayout(vulkanBindings.Data(), number_of_bindings);
}

/*
*	Destroys a render data table layout.
*/
void VulkanSubRenderingSystem::DestroyRenderDataTableLayout(RenderDataTableLayoutHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	SCOPED_LOCK(VulkanSubRenderingSystemData::_DestructionQueueLock);

	VulkanSubRenderingSystemData::_DestructionQueues[GetCurrentFramebufferIndex()].Emplace(VulkanSubRenderingSystemData::VulkanDestructionData::Type::RENDER_DATA_TABLE_LAYOUT, *handle);
}

/*
*	Creates a render data table.
*/
void VulkanSubRenderingSystem::CreateRenderDataTable(const RenderDataTableLayoutHandle render_data_table_layout, RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	*handle = VulkanInterface::Instance->CreateDescriptorSet(*static_cast<const VulkanDescriptorSetLayout *const RESTRICT>(render_data_table_layout));
}

/*
*	Binds an acceleration structure to a render data table.
*/
void VulkanSubRenderingSystem::BindAccelerationStructureToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const AccelerationStructureHandle acceleration_structure) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanAccelerationStructure *const RESTRICT vulkanAccelerationStructure{ static_cast<VulkanAccelerationStructure *const RESTRICT>(acceleration_structure) };

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
	writeDescriptorSet.dstArrayElement = array_element;
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
void VulkanSubRenderingSystem::BindCombinedImageSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image, const SamplerHandle sampler) const NOEXCEPT
{
	//Cache the Vulkan types.
	VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet{ static_cast<VulkanDescriptorSet *const RESTRICT>(*handle) };
	VulkanImage *const RESTRICT vulkanImage{ static_cast<VulkanImage *const RESTRICT>(image) };
	VulkanSampler *const RESTRICT vulkanSampler{ static_cast<VulkanSampler *const RESTRICT>(sampler) };

	//Create the destriptor image info.
	VkDescriptorImageInfo descriptorImageInfo;

	descriptorImageInfo.sampler = vulkanSampler->Get();

	switch (vulkanImage->GetType())
	{
		case VulkanImage::Type::VULKAN_2D_TEXTURE:
		case VulkanImage::Type::VULKAN_3D_TEXTURE:
		case VulkanImage::Type::VULKAN_CUBE_MAP_TEXTURE:
		{
			descriptorImageInfo.imageView = vulkanImage->GetImageView();

			break;
		}

		case VulkanImage::Type::VULKAN_RENDER_TARGET:
		{
			if (static_cast<VulkanRenderTarget *const RESTRICT>(vulkanImage)->GetSampleCount() > VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT)
			{
				descriptorImageInfo.imageView = static_cast<VulkanRenderTarget *const RESTRICT>(vulkanImage)->GetResolveImageView();
			}

			else
			{
				descriptorImageInfo.imageView = vulkanImage->GetImageView();
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	
	descriptorImageInfo.imageLayout = vulkanImage->GetImageLayout();

	//Create the write descriptor set.
	VkWriteDescriptorSet writeDescriptorSet;

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->Get();
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = array_element;
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
void VulkanSubRenderingSystem::BindSampledImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT
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
	writeDescriptorSet.dstArrayElement = array_element;
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
void VulkanSubRenderingSystem::BindSamplerToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const SamplerHandle sampler) const NOEXCEPT
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
	writeDescriptorSet.dstArrayElement = array_element;
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
void VulkanSubRenderingSystem::BindStorageImageToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const OpaqueHandle image) const NOEXCEPT
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
	writeDescriptorSet.dstArrayElement = array_element;
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
void VulkanSubRenderingSystem::BindStorageBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT
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
	writeDescriptorSet.dstArrayElement = array_element;
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
void VulkanSubRenderingSystem::BindUniformBufferToRenderDataTable(const uint32 binding, const uint32 array_element, RenderDataTableHandle *const RESTRICT handle, const BufferHandle buffer) const NOEXCEPT
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
	writeDescriptorSet.dstArrayElement = array_element;
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
void VulkanSubRenderingSystem::DestroyRenderDataTable(RenderDataTableHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	SCOPED_LOCK(VulkanSubRenderingSystemData::_DestructionQueueLock);

	VulkanSubRenderingSystemData::_DestructionQueues[GetCurrentFramebufferIndex()].Emplace(VulkanSubRenderingSystemData::VulkanDestructionData::Type::RENDER_DATA_TABLE, *handle);
}

/*
*	Creates a render target.
*/
void VulkanSubRenderingSystem::CreateRenderTarget(const Resolution resolution, const TextureFormat format, const SampleCount sample_count, RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the render target.
	*handle = static_cast<RenderTargetHandle>(VulkanInterface::Instance->CreateRenderTarget(VulkanTranslationUtilities::GetVulkanExtent(resolution),
																							VulkanTranslationUtilities::GetVulkanFormat(format),
																							VulkanTranslationUtilities::GetVulkanSampleCount(sample_count)));
}

/*
*	Destroys a render target.
*/
void VulkanSubRenderingSystem::DestroyRenderTarget(RenderTargetHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	SCOPED_LOCK(VulkanSubRenderingSystemData::_DestructionQueueLock);

	VulkanSubRenderingSystemData::_DestructionQueues[GetCurrentFramebufferIndex()].Emplace(VulkanSubRenderingSystemData::VulkanDestructionData::Type::RENDER_TARGET, *handle);
}

/*
*	Creates a sampler.
*/
void VulkanSubRenderingSystem::CreateSampler(const SamplerProperties &properties, SamplerHandle *const RESTRICT handle) const NOEXCEPT
{
	*handle = VulkanInterface::Instance->CreateSampler(	VulkanTranslationUtilities::GetVulkanTextureFilter(properties._MagnificationFilter),
														VulkanTranslationUtilities::GetVulkanMipmapMode(properties._MipmapMode),
														VulkanTranslationUtilities::GetVulkanAddressMode(properties._AddressMode),
														properties._AnisotropicSamples);
}

/*
*	Creates a texture 2D.
*/
void VulkanSubRenderingSystem::CreateTexture2D(const TextureData &data, Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the texture 2D.
	*handle = static_cast<Texture2DHandle>(VulkanInterface::Instance->Create2DTexture(	static_cast<uint32>(data._TextureDataContainer._TextureData.Size()),
																						data._TextureDataContainer._TextureWidth,
																						data._TextureDataContainer._TextureHeight,
																						data._TextureDataContainer._TextureChannels,
																						data._TextureDataContainer._TextureTexelSize,
																						data._TextureDataContainer._TextureData.Data(),
																						VulkanTranslationUtilities::GetVulkanFormat(data._TextureFormat),
																						VulkanTranslationUtilities::GetVulkanImageUsage(data._TextureUsage),
																						data._Updatable));
}

/*
*	Updates a texture 2D.
*/
void VulkanSubRenderingSystem::UpdateTexture2D(const TextureDataContainer &texture_data_container, Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	VulkanInterface::Instance->Update2DTexture(texture_data_container._TextureData.Data(), static_cast<Vulkan2DTexture* const RESTRICT>(*handle));
}

/*
*	Destroys a texture 2D.
*/
void VulkanSubRenderingSystem::DestroyTexture2D(Texture2DHandle *const RESTRICT handle) const NOEXCEPT
{
	//Put in a queue, destroy when no command buffer uses it anymore.
	SCOPED_LOCK(VulkanSubRenderingSystemData::_DestructionQueueLock);

	for (const DynamicArray<VulkanSubRenderingSystemData::VulkanDestructionData> &destruction_queue : VulkanSubRenderingSystemData::_DestructionQueues)
	{
		for (const VulkanSubRenderingSystemData::VulkanDestructionData &data : destruction_queue)
		{
			if (data._Type == VulkanSubRenderingSystemData::VulkanDestructionData::Type::TEXTURE_2D
				&& data._Handle == *handle)
			{
				ASSERT(false, "oh no");
			}
		}
	}

	VulkanSubRenderingSystemData::_DestructionQueues[GetCurrentFramebufferIndex()].Emplace(VulkanSubRenderingSystemData::VulkanDestructionData::Type::TEXTURE_2D, *handle);
}

/*
*	Creates a texture 3D.
*/
void VulkanSubRenderingSystem::CreateTexture3D(const TextureData &data, Texture3DHandle *const RESTRICT handle) const NOEXCEPT
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
*	Creates a texture cube.
*/
void VulkanSubRenderingSystem::CreateTextureCube(const uint32 resolution, const DynamicArray<DynamicArray<float32>> &data, TextureCubeHandle *const RESTRICT handle) const NOEXCEPT
{
	//Create the texture cube.
	*handle = static_cast<TextureCubeHandle>(VulkanInterface::Instance->CreateCubeMapTexture(data, resolution, resolution));
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Creates an ImGui texture.
*/
NO_DISCARD ImTextureID VulkanSubRenderingSystem::CreateImGuiTexture(const Texture2DHandle texture) NOEXCEPT
{
	//Retrieve the sample.
	const VulkanSampler *const RESTRICT sampler{ static_cast<const VulkanSampler *const RESTRICT>(RenderingSystem::Instance->GetSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::LINEAR, AddressMode::CLAMP_TO_EDGE, 0))) };

	//Cast to Vulkan image.
	const VulkanImage *const RESTRICT vulkan_image{ static_cast<const VulkanImage *const RESTRICT>(texture) };

	//Create the texture!
	return ImGui_ImplVulkan_AddTexture(sampler->Get(), vulkan_image->GetImageView(), vulkan_image->GetImageLayout());
}
#endif

/*
*	Initializes a pipeline
*/
void VulkanSubRenderingSystem::InitializePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT
{
	switch (pipeline->GetType())
	{
		case Pipeline::Type::COMPUTE:
		{
			VulkanSubRenderingSystemLogic::InitializeComputePipeline(static_cast<ComputePipeline *const RESTRICT>(pipeline));

			break;
		}

		case Pipeline::Type::GRAPHICS:
		{
			VulkanSubRenderingSystemLogic::InitializeGraphicsPipeline(static_cast<GraphicsPipeline *const RESTRICT>(pipeline));

			break;
		}

		case Pipeline::Type::RAY_TRACING:
		{
			if (RenderingSystem::Instance->IsRayTracingSupported())
			{
				VulkanSubRenderingSystemLogic::InitializeRayTracingPipeline(static_cast<RayTracingPipeline *const RESTRICT>(pipeline));
			}
			
			break;
		}
	}
}

/*
*	Terminates a pipeline
*/
void VulkanSubRenderingSystem::TerminatePipeline(Pipeline *const RESTRICT pipeline) const NOEXCEPT
{
	switch (pipeline->GetType())
	{
		case Pipeline::Type::COMPUTE:
		{
			VulkanSubRenderingSystemLogic::TerminateComputePipeline(static_cast<ComputePipeline *const RESTRICT>(pipeline));

			break;
		}

		case Pipeline::Type::GRAPHICS:
		{
			VulkanSubRenderingSystemLogic::TerminateGraphicsPipeline(static_cast<GraphicsPipeline *const RESTRICT>(pipeline));

			break;
		}

		case Pipeline::Type::RAY_TRACING:
		{
			if (RenderingSystem::Instance->IsRayTracingSupported())
			{
				VulkanSubRenderingSystemLogic::TerminateRayTracingPipeline(static_cast<RayTracingPipeline *const RESTRICT>(pipeline));
			}

			break;
		}
	}
}

/*
*	Takes an immedate screenshot and writes it to the given file path.
*/
void VulkanSubRenderingSystem::TakeImmediateScreenshot(const char *const RESTRICT file_path) NOEXCEPT
{
	//First of all, wait for all queues to finish their tasks.
	VulkanInterface::Instance->GetMainQueue()->WaitIdle();
	VulkanInterface::Instance->GetAsyncComputeQueue()->WaitIdle();
	VulkanInterface::Instance->GetAsyncTransferQueue()->WaitIdle();

	//Cache the Vulkan render target.
	VulkanRenderTarget *const RESTRICT vulkan_render_target{ static_cast<VulkanRenderTarget *const RESTRICT>(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE)) };

	//Calculate the image size.
	VkDeviceSize image_size{ RenderingSystem::Instance->GetFullResolution()._Width * RenderingSystem::Instance->GetFullResolution()._Height * sizeof(Vector4<float32>) };

	//Create a temporary CPU-accesible buffer and copy the data over there.
	VkBuffer temporary_buffer;
	VkDeviceMemory temporary_buffer_device_memory;

	VulkanUtilities::CreateVulkanBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, temporary_buffer, temporary_buffer_device_memory);

	//Copy the image to the buffer.
	VulkanUtilities::CopyImageToBuffer(RenderingSystem::Instance->GetFullResolution()._Width, RenderingSystem::Instance->GetFullResolution()._Height, vulkan_render_target->GetImage(), temporary_buffer);

	//Map the memory and copy it over into a Texture2D<Vector4<float32>>.
	Texture2D<Vector4<float32>> image{ RenderingSystem::Instance->GetFullResolution()._Width, RenderingSystem::Instance->GetFullResolution()._Height };

	void *data;
	VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), temporary_buffer_device_memory, 0, VK_WHOLE_SIZE, 0, &data));

	Memory::Copy(image.Data(), data, image_size);

	vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), temporary_buffer_device_memory);

	//Destroy the temporary buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), temporary_buffer, nullptr);
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), temporary_buffer_device_memory, nullptr);

	//Usually the alpha channel is set to non-one values due to alpha blending, which might look funky when exporting the image, so fix that.
	for (uint32 Y{ 0 }; Y < image.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < image.GetWidth(); ++X)
		{
			image.At(X, Y)._A = 1.0f;
		}
	}

	//Vulkan works with a flipped Y axis, so need to flip the image.
	Texture2D<Vector4<float32>> temporary_image{ image };

	for (uint32 Y{ 0 }; Y < image.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < image.GetWidth(); ++X)
		{
			image.At(X, Y) = temporary_image.At(X, image.GetHeight() - 1 - Y);
		}
	}

	//Write to file.
	switch (File::GetExtension(file_path))
	{
		case File::Extension::PNG:
		{
			PNGWriter::Write(image, file_path);

			break;
		}

		case File::Extension::TGA:
		{
			TGAWriter::Write(image, file_path);

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
*	Begins a frame.
*/
void VulkanSubRenderingSystem::BeginFrame() NOEXCEPT
{
	//Begin the frame for the platform.
	{
		PROFILING_SCOPE("VulkanSubRenderingSystem_BeginFrame");

		VulkanPlatform::BeginFrame();
	}

	//Pre-update the Vulkan interface.
	{
		PROFILING_SCOPE("VulkanSubRenderingSystem_VulkanInterface_PreUpdate");

		VulkanInterface::Instance->PreUpdate(VulkanSubRenderingSystemData::_FrameData.GetImageAvailableSemaphore());
	}

	//Set the current frame.
	{
		PROFILING_SCOPE("VulkanSubRenderingSystem_SetCurrentFrame");

		VulkanSubRenderingSystemData::_FrameData.SetCurrentFrame(VulkanInterface::Instance->GetSwapchain().GetCurrentImageIndex());
	}

	//Wait for the current fence to finish.
	{
		PROFILING_SCOPE("VulkanSubRenderingSystem_WaitForCurrentFence");

		VulkanSubRenderingSystemData::_FrameData.GetCurrentFence()->WaitFor();
	}

	//Reset the current fence.
	{
		PROFILING_SCOPE("VulkanSubRenderingSystem_ResetCurrentFence");

		VulkanSubRenderingSystemData::_FrameData.GetCurrentFence()->Reset();
	}

	//Process the destruction queue.
	{
		PROFILING_SCOPE("VulkanSubRenderingSystem_ProcessDestructionQueue");

		VulkanSubRenderingSystemLogic::ProcessDestructionQueue(GetCurrentFramebufferIndex());
	}
}

/*
*	Ends a frame.
*/
void VulkanSubRenderingSystem::EndFrame(const CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT
{
	//Submit current command buffer.
	VulkanInterface::Instance->GetMainQueue()->Submit(	*static_cast<const VulkanCommandBuffer *const RESTRICT>(frame_command_buffer->GetCommandBufferData()),
#if VULKAN_RECEIVES_SWAPCHAIN_FROM_PLATFORM
														0,
														nullptr,
#else
														1,
														VulkanSubRenderingSystemData::_FrameData.GetImageAvailableSemaphore(),
#endif
														VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
														1,
														VulkanSubRenderingSystemData::_FrameData.GetRenderFinishedSemaphore(),
														VulkanSubRenderingSystemData::_FrameData.GetCurrentFence()->Get());

	//Post-update the Vulkan interface.
	VulkanInterface::Instance->PostUpdate(VulkanSubRenderingSystemData::_FrameData.GetRenderFinishedSemaphore());
}
#endif