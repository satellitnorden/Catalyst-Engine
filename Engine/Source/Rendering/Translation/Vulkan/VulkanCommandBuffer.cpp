#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Native/CommandBuffer.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandBufferAbstraction.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>
#include <Rendering/Translation/Vulkan/VulkanComputePipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanGraphicsPipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanRayTracingPipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

//Third party.
#if defined(CATALYST_EDITOR)
#include <ThirdParty/imgui.h>
#include <ThirdParty/imgui_impl_vulkan.h>
#endif

/*
*	Begins the command buffer.
*/
void CommandBuffer::Begin(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	if (pipeline)
	{
		if (pipeline->GetType() == Pipeline::Type::COMPUTE)
		{
			//Begin the command buffer.
			reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE);
		}

		else if (pipeline->GetType() == Pipeline::Type::GRAPHICS)
		{
			//Cache the Vulkan graphics pipeline data.
			const VulkanGraphicsPipelineData *const RESTRICT pipeline_data{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetData()) };

			//Begin the command buffer.
			reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipeline_data->_RenderPass->Get(), 0, static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->IsRenderingDirectlyToScreen() ? pipeline_data->_FrameBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()]->Get() : pipeline_data->_FrameBuffers[0]->Get());
		}

		else if (pipeline->GetType() == Pipeline::Type::RAY_TRACING)
		{
			//Begin the command buffer.
			reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE);
		}
	}

	else
	{
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	}
}

/*
*	Binds an index buffer.
*/
void CommandBuffer::BindIndexBuffer(const Pipeline *const RESTRICT pipeline, BufferHandle buffer, const uint64 offset) NOEXCEPT
{
	//Bind the index buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindIndexBuffer(static_cast<VulkanBuffer *const RESTRICT>(buffer)->Get(), offset);
}

/*
*	Binds a pipeline.
*/
void CommandBuffer::BindPipeline(const Pipeline* const RESTRICT pipeline) NOEXCEPT
{
	if (pipeline->GetType() == Pipeline::Type::COMPUTE)
	{
		//Cache the pipeline data.
		const VulkanComputePipelineData *const RESTRICT pipeline_data{ static_cast<const VulkanComputePipelineData *const RESTRICT>(pipeline->GetData()) };

		//Bind the pipeline.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindPipeline(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_data->_Pipeline->GetPipeline());
	}

	else if (pipeline->GetType() == Pipeline::Type::GRAPHICS)
	{
		//Cache the pipeline data.
		const VulkanGraphicsPipelineData *const RESTRICT pipeline_data{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetData()) };

		//Bind the pipeline.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindPipeline(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_data->_Pipeline->Get());
	}

	else if (pipeline->GetType() == Pipeline::Type::RAY_TRACING)
	{
		//Cache the pipeline data.
		const VulkanRayTracingPipelineData *const RESTRICT pipeline_data{ static_cast<const VulkanRayTracingPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Bind the pipeline.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindPipeline(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, pipeline_data->_Pipeline->GetPipeline());
	}
}

/*
*	Binds a render data table.
*/
void CommandBuffer::BindRenderDataTable(const Pipeline *const RESTRICT pipeline, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT
{
	if (pipeline->GetType() == Pipeline::Type::COMPUTE)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanComputePipelineData *const RESTRICT pipelineData{ static_cast<const VulkanComputePipelineData *const RESTRICT>(pipeline->GetData()) };

		//Bind the render data tables.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindDescriptorSets(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pipelineData->_Pipeline->GetPipelineLayout(), binding, 1, &static_cast<const VulkanDescriptorSet *const RESTRICT>(renderDataTable)->Get());
	}

	else if (pipeline->GetType() == Pipeline::Type::GRAPHICS)
	{
		//Cache the Vulkan graphics pipeline data.
		const VulkanGraphicsPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Bind the render data tables.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindDescriptorSets(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->_Pipeline->GetPipelineLayout(), binding, 1, &static_cast<const VulkanDescriptorSet *const RESTRICT>(renderDataTable)->Get());
	}

	else if (pipeline->GetType() == Pipeline::Type::RAY_TRACING)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanRayTracingPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanRayTracingPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Bind the render data tables.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindDescriptorSets(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, pipelineData->_Pipeline->GetPipelineLayout(), binding, 1, &static_cast<const VulkanDescriptorSet *const RESTRICT>(renderDataTable)->Get());
	}
}

/*
*	Binds a number of vertex buffers.
*/
void CommandBuffer::BindVertexBuffer(const Pipeline *const RESTRICT pipeline, const uint32 binding, BufferHandle buffer, const uint64 *const RESTRICT offset) NOEXCEPT
{
	//Bind the vertex buffers.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindVertexBuffers(binding, 1, &static_cast<const VulkanBuffer *const RESTRICT>(buffer)->Get(), reinterpret_cast<const VkDeviceSize *const RESTRICT>(offset));
}

/*
*	Blits an image.
*/
void CommandBuffer::BlitImage(const Pipeline *const RESTRICT pipeline, const OpaqueHandle source, const OpaqueHandle target) NOEXCEPT
{
	VulkanImage *const RESTRICT source_image{ static_cast<VulkanImage* const RESTRICT>(source) };
	VulkanImage *const RESTRICT target_image{ static_cast<VulkanImage* const RESTRICT>(source) };

	static_cast<VulkanCommandBuffer* const RESTRICT>(_CommandBufferData)->CommandBlitImage(source_image->GetImage(), target_image->GetImage());
}

/*
*	Dispatches.
*/
void CommandBuffer::Dispatch(const Pipeline *const RESTRICT pipeline, const uint32 width, const uint32 height, const uint32 depth) NOEXCEPT
{
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandDispatch(width, height, depth);
}

/*
*	Draws.
*/
void CommandBuffer::Draw(const Pipeline *const RESTRICT pipeline, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandDraw(vertexCount, instanceCount);
}

#if defined(CATALYST_EDITOR)
/*
*	Draws the current ImGui state.
*/
void CommandBuffer::DrawImGui(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<VulkanCommandBuffer* const RESTRICT>(_CommandBufferData)->Get());
}
#endif

/*
*	Draws indexed.
*/
void CommandBuffer::DrawIndexed(const Pipeline *const RESTRICT pipeline, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw indexed.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandDrawIndexed(indexCount, instanceCount);
}

/*
*	Executes commands from a secondary command buffer.
*/
void CommandBuffer::ExecuteCommands(const Pipeline *const RESTRICT pipeline, const CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	if (pipeline->GetType() == Pipeline::Type::GRAPHICS)
	{
		//Cache the Vulkan graphics pipeline data.
		const VulkanGraphicsPipelineData *const RESTRICT pipeline_data{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetData()) };

		if (static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetDepthStencilAttachmentLoadOperator() == AttachmentLoadOperator::CLEAR
			|| static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetColorAttachmentLoadOperator() == AttachmentLoadOperator::CLEAR)
		{
			Vector4<float32> clear_color;
			float32 depth_value;

			if (pipeline_data->_Extent.width == CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution)
			{
				clear_color = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);
				depth_value = 1.0f;
			}

			else
			{
				clear_color = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
				depth_value = 0.0f;
			}

			reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBeginRenderPassAndClear
			(
			clear_color,
			depth_value,
			pipeline_data->_RenderPass->Get(),
			pipeline_data->_FrameBuffers[pipeline_data->_RenderToScreeen ? RenderingSystem::Instance->GetCurrentFramebufferIndex() : 0]->Get(),
			pipeline_data->_RenderToScreeen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : pipeline_data->_Extent,
			VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS,
			pipeline_data->_NumberOfAttachments
			);
		}

		else
		{
			reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBeginRenderPass
			(
			pipeline_data->_RenderPass->Get(),
			pipeline_data->_FrameBuffers[pipeline_data->_RenderToScreeen ? RenderingSystem::Instance->GetCurrentFramebufferIndex() : 0]->Get(),
			pipeline_data->_RenderToScreeen ? VulkanInterface::Instance->GetSwapchain().GetSwapExtent() : pipeline_data->_Extent,
			VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
			);
		}
	}

	//Record the execution of the secondary command buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandExecuteCommands(reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(pipeline->GetCommandBuffer()->GetCommandBufferData())->Get());

	if (pipeline->GetType() == Pipeline::Type::GRAPHICS)
	{
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandEndRenderPass();
	}

	//Insert image barriers.
	if (true)
	{
		if (pipeline->GetType() == Pipeline::Type::GRAPHICS
			&& !static_cast<const GraphicsPipeline* const RESTRICT>(pipeline)->IsRenderingDirectlyToScreen())
		{
			for (const RenderTargetHandle render_target : static_cast<const GraphicsPipeline* const RESTRICT>(pipeline)->GetOutputRenderTargets())
			{
				VulkanRenderTarget *const RESTRICT vulkan_render_target{ static_cast<VulkanRenderTarget* const RESTRICT>(render_target) };

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
				vkCmdPipelineBarrier(reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->Get(), VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
			}
		}
	}
}

/*
*	Establishes an image memory barrier.
*/
void CommandBuffer::ImageMemoryBarrier(	const Pipeline* const RESTRICT pipeline,
										const AccessFlags source_access_mask,
										const AccessFlags destination_access_mask,
										const ImageLayout old_layout,
										const ImageLayout new_layout,
										const OpaqueHandle image,
										const PipelineStageFlags source_stage_mask,
										const PipelineStageFlags destination_stage_mask) NOEXCEPT
{
	//Record the command.
	reinterpret_cast<VulkanCommandBuffer* const RESTRICT>(_CommandBufferData)->CommandImageMemoryBarrier
	(
		VulkanTranslationUtilities::GetVulkanAccessFlags(source_access_mask),
		VulkanTranslationUtilities::GetVulkanAccessFlags(destination_access_mask),
		VulkanTranslationUtilities::GetVulkanImageLayout(old_layout),
		VulkanTranslationUtilities::GetVulkanImageLayout(new_layout),
		static_cast<VulkanImage* const RESTRICT>(image)->GetImage(),
		VulkanTranslationUtilities::GetVulkanPipelineStageFlags(source_stage_mask),
		VulkanTranslationUtilities::GetVulkanPipelineStageFlags(destination_stage_mask)
	);
}

/*
*	Pushes constants.
*/
void CommandBuffer::PushConstants(const Pipeline *const RESTRICT pipeline, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT
{
	if (pipeline->GetType() == Pipeline::Type::COMPUTE)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanComputePipelineData *const RESTRICT pipelineData{ static_cast<const VulkanComputePipelineData *const RESTRICT>(pipeline->GetData()) };

		//Push the constants.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandPushConstants(pipelineData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
	}

	else if (pipeline->GetType() == Pipeline::Type::GRAPHICS)
	{
		//Cache the Vulkan graphics pipeline data.
		const VulkanGraphicsPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Push the constants.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandPushConstants(pipelineData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
	}

	else if (pipeline->GetType() == Pipeline::Type::RAY_TRACING)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanRayTracingPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanRayTracingPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Push the constants.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandPushConstants(pipelineData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
	}
}

/*
*	Sets an event.
*/
void CommandBuffer::SetEvent(const Pipeline *const RESTRICT pipeline, const EventHandle event) NOEXCEPT
{
	reinterpret_cast<VulkanCommandBuffer* const RESTRICT>(_CommandBufferData)->CommandSetEvent(	static_cast<VulkanEvent* const RESTRICT>(event)->Get(),
																								VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
}

/*
*	Traces rays.
*/
void CommandBuffer::TraceRays(const Pipeline *const RESTRICT pipeline, const uint32 width, const uint32 height) NOEXCEPT
{
	const VulkanRayTracingPipelineData *const RESTRICT pipeline_data{ static_cast<const VulkanRayTracingPipelineData *const RESTRICT>(pipeline->GetData()) };

	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandTraceRays(pipeline_data->_ShaderBindingTableBuffer->Get(),
																								pipeline_data->_MissShaderBindingOffset,
																								pipeline_data->_MissShaderBindingStride,
																								pipeline_data->_HitShaderBindingOffset,
																								pipeline_data->_HitShaderBindingStride,
																								width,
																								height);
}

/*
*	Writes a timestamp.
*/
void CommandBuffer::WriteTimestamp(const Pipeline *const RESTRICT pipeline, const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT
{
	if (query_index == 0)
	{
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandResetQueryPool(static_cast<const VulkanQueryPool *const RESTRICT>(query_pool)->Get());
	}

	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandWriteTimestamp(	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
																										static_cast<const VulkanQueryPool *const RESTRICT>(query_pool)->Get(),
																										query_index);
}

/*
*	Ends the command buffer.
*/
void CommandBuffer::End(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	//End the command buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->End();
}
#endif