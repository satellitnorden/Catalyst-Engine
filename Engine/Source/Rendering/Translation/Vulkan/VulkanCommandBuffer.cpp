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
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <ThirdParty/ImGui/imgui.h>
#include <ThirdParty/ImGui/imgui_impl_vulkan.h>
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
	VulkanImage *const RESTRICT target_image{ static_cast<VulkanImage* const RESTRICT>(target) };

	ASSERT(source_image->GetExtent().width == target_image->GetExtent().width, "Mismatching width!");
	ASSERT(source_image->GetExtent().height == target_image->GetExtent().height, "Mismatching height!");
	ASSERT(source_image->GetExtent().depth == target_image->GetExtent().depth, "Mismatching depth!");

	static_cast<VulkanCommandBuffer* const RESTRICT>(_CommandBufferData)->CommandBlitImage(source_image->GetImage(), target_image->GetImage(), VkExtent2D(source_image->GetExtent().width, source_image->GetExtent().height));
}

/*
*	Clears a depth/stencil image.
*/
void CommandBuffer::ClearDepthStencilImage(const OpaqueHandle image) NOEXCEPT
{
	VkCommandBuffer command_buffer{ static_cast<VulkanCommandBuffer* const RESTRICT>(_CommandBufferData)->Get() };
	VulkanImage *const RESTRICT _image{ static_cast<VulkanImage *const RESTRICT>(image) };

	//Transition to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
	{
		VkImageMemoryBarrier barrier{ };

		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		barrier.oldLayout = _image->GetImageLayout();
		barrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = _image->GetImage();

		barrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		//Record the pipeline barrier command.
		vkCmdPipelineBarrier
		(
			command_buffer,
			VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);
	}

	//Actually clear the image.
	{
		VkClearDepthStencilValue clear_value;

		clear_value.depth = 0.0f;
		clear_value.stencil = 0;

		VkImageSubresourceRange range;

		range.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		vkCmdClearDepthStencilImage
		(
			command_buffer,
			_image->GetImage(),
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			&clear_value,
			1,
			&range
		);
	}

	//Transition back to the original image layout.
	{
		VkImageMemoryBarrier barrier{ };

		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

		barrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = _image->GetImageLayout();
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = _image->GetImage();

		barrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		//Record the pipeline barrier command.
		vkCmdPipelineBarrier
		(
			command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);
	}
}

/*
*	Clears a color image.
*/
void CommandBuffer::ClearColorImage(const OpaqueHandle image) NOEXCEPT
{
	VkCommandBuffer command_buffer{ static_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->Get() };
	VulkanImage *const RESTRICT _image{ static_cast<VulkanImage *const RESTRICT>(image) };

	//Transition to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
	{
		VkImageMemoryBarrier barrier{ };

		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		barrier.oldLayout = _image->GetImageLayout();
		barrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = _image->GetImage();

		barrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		//Record the pipeline barrier command.
		vkCmdPipelineBarrier
		(
			command_buffer,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);
	}

	//Actually clear the image.
	{
		VkClearColorValue clear_value;
		memset(&clear_value, 0, sizeof(VkClearColorValue));

		VkImageSubresourceRange range;

		range.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		vkCmdClearColorImage
		(
			command_buffer,
			_image->GetImage(),
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			&clear_value,
			1,
			&range
		);
	}

	//Transition back to the original image layout.
	{
		VkImageMemoryBarrier barrier{ };

		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

		barrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = _image->GetImageLayout();
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = _image->GetImage();

		barrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		//Record the pipeline barrier command.
		vkCmdPipelineBarrier
		(
			command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);
	}

	//If this is a render target, check if the resolve image needs to be cleared as well.
	if (_image->GetType() == VulkanImage::Type::VULKAN_RENDER_TARGET)
	{
		VulkanRenderTarget *const RESTRICT _render_target{ static_cast<VulkanRenderTarget *const RESTRICT>(image) };

		if (_render_target->GetSampleCount() > VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT)
		{
			//Transition to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
			{
				VkImageMemoryBarrier barrier{ };

				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.pNext = nullptr;
				barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				barrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
				barrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = _render_target->GetResolveImage();

				barrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseMipLevel = 0;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 1;

				//Record the pipeline barrier command.
				vkCmdPipelineBarrier
				(
					command_buffer,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier
				);
			}

			//Actually clear the image.
			{
				VkClearColorValue clear_value;
				memset(&clear_value, 0, sizeof(VkClearColorValue));

				VkImageSubresourceRange range;

				range.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
				range.baseMipLevel = 0;
				range.levelCount = 1;
				range.baseArrayLayer = 0;
				range.layerCount = 1;

				vkCmdClearColorImage
				(
					command_buffer,
					_render_target->GetResolveImage(),
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					&clear_value,
					1,
					&range
				);
			}

			//Transition back to the original image layout.
			{
				VkImageMemoryBarrier barrier{ };

				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.pNext = nullptr;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

				barrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = _render_target->GetResolveImage();

				barrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseMipLevel = 0;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 1;

				//Record the pipeline barrier command.
				vkCmdPipelineBarrier
				(
					command_buffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier
				);
			}
		}
	}
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
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
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandImageMemoryBarrier
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
*	Writes a begin timestamp.
*/
void CommandBuffer::WriteBeginTimestamp(const Pipeline *const RESTRICT pipeline, const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT
{
	if (query_index == 0)
	{
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandResetQueryPool(static_cast<const VulkanQueryPool *const RESTRICT>(query_pool)->Get(), static_cast<const VulkanQueryPool* const RESTRICT>(query_pool)->GetQueryCount());
	}

	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandWriteTimestamp(	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
																										static_cast<const VulkanQueryPool *const RESTRICT>(query_pool)->Get(),
																										query_index);
}

/*
*	Writes an end timestamp.
*/
void CommandBuffer::WriteEndTimestamp(const Pipeline *const RESTRICT pipeline, const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT
{
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandWriteTimestamp(	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
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