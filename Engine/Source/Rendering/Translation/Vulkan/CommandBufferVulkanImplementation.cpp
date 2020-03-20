#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Native/CommandBuffer.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracingPipeline.h>
#include <Rendering/Translation/Vulkan/VulkanComputePipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanGraphicsPipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanRayTracingPipelineData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Begins the command buffer.
*/
void CommandBuffer::Begin(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	if (pipeline->GetType() == Pipeline::Type::Compute)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanComputePipelineData *const RESTRICT pipelineData{ static_cast<const VulkanComputePipelineData *const RESTRICT>(pipeline->GetData()) };

		//Begin the command buffer.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE);

		//Bind the pipeline.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindPipeline(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pipelineData->_Pipeline->GetPipeline());
	}

	else if (pipeline->GetType() == Pipeline::Type::Graphics)
	{
		//Cache the Vulkan graphics pipeline data.
		const VulkanGraphicsPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetData()) };

		//Begin the command buffer.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelineData->_RenderPass->Get(), 0, static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetRenderTargets().Empty() ? pipelineData->_FrameBuffers[0]->Get() : static_cast<const GraphicsPipeline *const RESTRICT>(pipeline)->GetRenderTargets()[0] == RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCREEN) ? pipelineData->_FrameBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()]->Get() : pipelineData->_FrameBuffers[0]->Get());

		//Bind the pipeline.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindPipeline(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->_Pipeline->Get());
	}
	
	else if (pipeline->GetType() == Pipeline::Type::RayTracing)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanRayTracingPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanRayTracingPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Begin the command buffer.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE);

		//Bind the pipeline.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindPipeline(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, pipelineData->_Pipeline->GetPipeline());
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
*	Binds a render data table.
*/
void CommandBuffer::BindRenderDataTable(const Pipeline *const RESTRICT pipeline, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT
{
	if (pipeline->GetType() == Pipeline::Type::Compute)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanComputePipelineData *const RESTRICT pipelineData{ static_cast<const VulkanComputePipelineData *const RESTRICT>(pipeline->GetData()) };

		//Bind the render data tables.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindDescriptorSets(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pipelineData->_Pipeline->GetPipelineLayout(), binding, 1, &static_cast<const VulkanDescriptorSet *const RESTRICT>(renderDataTable)->Get());
	}

	else if (pipeline->GetType() == Pipeline::Type::Graphics)
	{
		//Cache the Vulkan graphics pipeline data.
		const VulkanGraphicsPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Bind the render data tables.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindDescriptorSets(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->_Pipeline->GetPipelineLayout(), binding, 1, &static_cast<const VulkanDescriptorSet *const RESTRICT>(renderDataTable)->Get());
	}

	else if (pipeline->GetType() == Pipeline::Type::RayTracing)
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

/*
*	Draws indexed.
*/
void CommandBuffer::DrawIndexed(const Pipeline *const RESTRICT pipeline, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw indexed.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandDrawIndexed(indexCount, instanceCount);
}

/*
*	Establishes an image memory barrier.
*/
void CommandBuffer::ImageMemoryBarrier(const Pipeline* const RESTRICT pipeline, const OpaqueHandle image) NOEXCEPT
{
	//Retrieve the Vulkan command buffer.
	VulkanCommandBuffer* const RESTRICT vulkan_command_buffer{ static_cast<VulkanCommandBuffer* const RESTRICT>(_CommandBufferData) };

	//Retrieve the Vulkan image.
	VulkanImage* const RESTRICT vulkan_image{ static_cast<VulkanImage* const RESTRICT>(image) };

	//Create the image memory barrier.
	VkImageMemoryBarrier image_memory_barrier;

	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = nullptr;
	image_memory_barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	image_memory_barrier.dstAccessMask =	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
											| VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	image_memory_barrier.oldLayout = vulkan_image->GetImageLayout();
	image_memory_barrier.newLayout = vulkan_image->GetImageLayout();
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.image = vulkan_image->GetImage();

	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = 1;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;
	image_memory_barrier.subresourceRange.layerCount = 1;

	//Record the pipeline barrier command.
	vkCmdPipelineBarrier(vulkan_command_buffer->Get(), VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
}

/*
*	Pushes constants.
*/
void CommandBuffer::PushConstants(const Pipeline *const RESTRICT pipeline, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT
{
	if (pipeline->GetType() == Pipeline::Type::Compute)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanComputePipelineData *const RESTRICT pipelineData{ static_cast<const VulkanComputePipelineData *const RESTRICT>(pipeline->GetData()) };


		//Push the constants.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandPushConstants(pipelineData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
	}

	else if (pipeline->GetType() == Pipeline::Type::Graphics)
	{
		//Cache the Vulkan graphics pipeline data.
		const VulkanGraphicsPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanGraphicsPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Push the constants.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandPushConstants(pipelineData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
	}

	else if (pipeline->GetType() == Pipeline::Type::RayTracing)
	{
		//Cache the Vulkan ray tracing pipeline data.
		const VulkanRayTracingPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanRayTracingPipelineData *const RESTRICT>(pipeline->GetData()) };

		//Push the constants.
		reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandPushConstants(pipelineData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
	}
}

/*
*	Traces rays.
*/
void CommandBuffer::TraceRays(const Pipeline *const RESTRICT pipeline, const uint32 width, const uint32 height) NOEXCEPT
{
	const VulkanRayTracingPipelineData *const RESTRICT pipelineData{ static_cast<const VulkanRayTracingPipelineData *const RESTRICT>(pipeline->GetData()) };

	const uint32 stride{ VulkanInterface::Instance->GetPhysicalDevice().GetRayTracingProperties().shaderGroupHandleSize };

	const uint32 missShaderBindingOffset{ static_cast<const RayTracingPipeline *const RESTRICT>(pipeline)->GetClosestHitShader() != Shader::None ? stride * 2 : stride };
	const uint32 hitShaderBindingOffset{ stride };

	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandTraceRays(pipelineData->_ShaderBindingTableBuffer->Get(), missShaderBindingOffset, hitShaderBindingOffset, width, height);
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