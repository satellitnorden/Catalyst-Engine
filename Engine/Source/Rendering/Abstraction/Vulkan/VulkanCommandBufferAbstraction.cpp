#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanCommandBufferAbstraction.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

//Vulkan command buffer constants.
namespace VulkanCommandBufferConstants
{
	constexpr uint64 MAXIMUM_NUMBER_OF_CLEAR_VALUES{ 7 };
}

/*
*	Initializes this Vulkan command buffer.
*/
void VulkanCommandBuffer::Initialize(const VulkanCommandPool &vulkanCommandPool, const VkCommandBufferLevel level) NOEXCEPT
{
	//Create the command buffer allocate info.
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	CreateCommandBufferAllocateInfo(commandBufferAllocateInfo, vulkanCommandPool, level);

	//Allocate the command buffer!
	VULKAN_ERROR_CHECK(vkAllocateCommandBuffers(VulkanInterface::Instance->GetLogicalDevice().Get(), &commandBufferAllocateInfo, &_VulkanCommandBuffer));
}

/*
*	Begins this Vulkan command buffer as a primary command buffer.
*/
void VulkanCommandBuffer::BeginPrimary(const VkCommandBufferUsageFlags commandBufferUsageFlags) NOEXCEPT
{
	//Create the command buffer begin info.
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	CreatePrimaryCommandBufferBeginInfo(commandBufferBeginInfo, commandBufferUsageFlags);

	//Begin the command buffer!
	VULKAN_ERROR_CHECK(vkBeginCommandBuffer(_VulkanCommandBuffer, &commandBufferBeginInfo));
}

/*
*	Begins this Vulkan command buffer as a secondary command buffer.
*/
void VulkanCommandBuffer::BeginSecondary(const VkCommandBufferUsageFlags commandBufferUsageFlags, const VkRenderPass renderPass, const uint32 subpass, const VkFramebuffer framebuffer) NOEXCEPT
{
	//Create the command buffer inheritance info.
	VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
	CreateCommandBufferInheritanceInfo(commandBufferInheritanceInfo, renderPass, subpass, framebuffer);

	//Create the command buffer begin info.
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	CreateSecondaryCommandBufferBeginInfo(commandBufferBeginInfo, commandBufferUsageFlags, &commandBufferInheritanceInfo);

	//Begin the command buffer!
	VULKAN_ERROR_CHECK(vkBeginCommandBuffer(_VulkanCommandBuffer, &commandBufferBeginInfo));
}

/*
*	Records a begin render pass command.
*/
void VulkanCommandBuffer::CommandBeginRenderPass(const VkRenderPass renderPass, const VkFramebuffer framebuffer, const VkExtent2D renderArea, const VkSubpassContents contents) NOEXCEPT
{
	VkRenderPassBeginInfo renderPassBeginInfo;

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = renderArea;
	renderPassBeginInfo.clearValueCount = 0;
	renderPassBeginInfo.pClearValues = nullptr;

	vkCmdBeginRenderPass(_VulkanCommandBuffer, &renderPassBeginInfo, contents);
}

/*
*	Records a begin render pass command and clears.
*/
void VulkanCommandBuffer::CommandBeginRenderPassAndClear(const Vector4<float> &color, const float depth, const VkRenderPass renderPass, const VkFramebuffer framebuffer, const VkExtent2D renderArea, const VkSubpassContents contents, const uint32 numberOfClearValues) NOEXCEPT
{
	StaticArray<VkClearValue, VulkanCommandBufferConstants::MAXIMUM_NUMBER_OF_CLEAR_VALUES> clearValues;

	for (uint32 i = 0; i < numberOfClearValues; ++i)
	{
		if (i == 0)
		{
			clearValues[i].depthStencil = { depth, 0 };
		}

		else
		{
			clearValues[i].color = { color._X, color._Y, color._Z, color._W };
		}
	}

	VkRenderPassBeginInfo renderPassBeginInfo;

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = renderArea;
	renderPassBeginInfo.clearValueCount = numberOfClearValues;
	renderPassBeginInfo.pClearValues = clearValues.Data();

	vkCmdBeginRenderPass(_VulkanCommandBuffer, &renderPassBeginInfo, contents);
}

/*
*	Records a bind descriptor sets command.
*/
void VulkanCommandBuffer::CommandBindDescriptorSets(const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout pipelineLayout, const uint32 firstBinding, const uint32 descriptorSetCount, const VkDescriptorSet *RESTRICT descriptorSets) NOEXCEPT
{
	vkCmdBindDescriptorSets(_VulkanCommandBuffer, pipelineBindPoint, pipelineLayout, firstBinding, descriptorSetCount, descriptorSets, 0, nullptr);
}

/*
*	Records a bind index buffer command.
*/
void VulkanCommandBuffer::CommandBindIndexBuffer(const VkBuffer vulkanIndexBuffer, const VkDeviceSize offset) NOEXCEPT
{
	vkCmdBindIndexBuffer(_VulkanCommandBuffer, vulkanIndexBuffer, offset, VK_INDEX_TYPE_UINT32);
}

/*
*	Records a bind pipeline command.
*/
void VulkanCommandBuffer::CommandBindPipeline(const VkPipelineBindPoint pipelineBindPoint, const VkPipeline vulkanPipeline) NOEXCEPT
{
	vkCmdBindPipeline(_VulkanCommandBuffer, pipelineBindPoint, vulkanPipeline);
}

/*
*	Records a bind vertex buffers command.
*/
void VulkanCommandBuffer::CommandBindVertexBuffers(const uint32 firstBinding, const uint32 vertexBufferCount, const VkBuffer *RESTRICT vertexBuffers, const VkDeviceSize *RESTRICT offsets) NOEXCEPT
{
	vkCmdBindVertexBuffers(_VulkanCommandBuffer, firstBinding, vertexBufferCount, vertexBuffers, offsets);
}

/*
*	Records a blit image command.
*/
void VulkanCommandBuffer::CommandBlitImage(VkImage source, VkImage destination) NOEXCEPT
{
	VkImageBlit image_blit;

	image_blit.srcSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
	image_blit.srcSubresource.mipLevel = 0;
	image_blit.srcSubresource.baseArrayLayer = 0;
	image_blit.srcSubresource.layerCount = 1;
	image_blit.srcOffsets[0] = VkOffset3D{ 0, 0, 0 };
	image_blit.srcOffsets[1] = VkOffset3D{ 0, 0, 0 };
	image_blit.dstSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
	image_blit.dstSubresource.mipLevel = 0;
	image_blit.dstSubresource.baseArrayLayer = 0;
	image_blit.dstSubresource.layerCount = 1;
	image_blit.dstOffsets[0] = VkOffset3D{ 0, 0, 0 };
	image_blit.dstOffsets[1] = VkOffset3D{ 0, 0, 0 };

	vkCmdBlitImage(_VulkanCommandBuffer, source, VkImageLayout::VK_IMAGE_LAYOUT_GENERAL, destination, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, 1, &image_blit, VkFilter::VK_FILTER_NEAREST);
}

/*
*	Records a build acceleration structure command.
*/
void VulkanCommandBuffer::CommandBuildAccelerationStructure
(
	const VkAccelerationStructureTypeNV type,
	const uint32 instance_count,
	const VkBuffer instance_buffer,
	const uint32 geometry_count,
	const VkGeometryNV* const RESTRICT geometries,
	const VkAccelerationStructureNV acceleration_structure,
	const VkBuffer scratch_buffer
) NOEXCEPT
{
	VkAccelerationStructureInfoNV acceleration_structure_info;

	acceleration_structure_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
	acceleration_structure_info.pNext = nullptr;
	acceleration_structure_info.type = type;
	acceleration_structure_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;
	acceleration_structure_info.instanceCount = instance_count;
	acceleration_structure_info.geometryCount = geometry_count;
	acceleration_structure_info.pGeometries = geometries;

	vkCmdBuildAccelerationStructureNV
	(
		_VulkanCommandBuffer,
		&acceleration_structure_info,
		instance_buffer,
		0,
		VK_FALSE,
		acceleration_structure,
		VK_NULL_HANDLE,
		scratch_buffer,
		0
	);
}

/*
*	Records a copy buffer command.
*/
void VulkanCommandBuffer::CommandCopyBuffer(const VkBuffer source, VkBuffer destination, const VkDeviceSize size) NOEXCEPT
{
	VkBufferCopy bufferCopy;

	bufferCopy.srcOffset = 0;
	bufferCopy.dstOffset = 0;
	bufferCopy.size = size;

	vkCmdCopyBuffer(_VulkanCommandBuffer, source, destination, 1, &bufferCopy);
}

/*
*	Records a dispatch command.
*/
void VulkanCommandBuffer::CommandDispatch(const uint32 width, const uint32 height, const uint32 depth) NOEXCEPT
{
	vkCmdDispatch(_VulkanCommandBuffer, width, height, depth);
}

/*
*	Records a draw command.
*/
void VulkanCommandBuffer::CommandDraw(const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{
	vkCmdDraw(_VulkanCommandBuffer, vertexCount, instanceCount, 0, 0);
}

/*
*	Records a draw indexed command.
*/
void VulkanCommandBuffer::CommandDrawIndexed(const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	vkCmdDrawIndexed(_VulkanCommandBuffer, indexCount, instanceCount, 0, 0, 0);
}

/*
*	Records an end render pass command.
*/
void VulkanCommandBuffer::CommandEndRenderPass() NOEXCEPT
{
	vkCmdEndRenderPass(_VulkanCommandBuffer);
}

/*
*	Records an execute commands command.
*/
void VulkanCommandBuffer::CommandExecuteCommands(const VkCommandBuffer commandBuffer) NOEXCEPT
{
	vkCmdExecuteCommands(_VulkanCommandBuffer, 1, &commandBuffer);
}

/*
*	Records an image memory barrier command.
*/
void VulkanCommandBuffer::CommandImageMemoryBarrier(const VkAccessFlags source_access_mask,
													const VkAccessFlags destination_access_mask,
													const VkImageLayout old_layout,
													const VkImageLayout new_layout,
													const VkImage image,
													const VkPipelineStageFlags source_stage_mask,
													const VkPipelineStageFlags destination_stage_mask) NOEXCEPT
{
	//Create the image memory barrier.
	VkImageMemoryBarrier image_memory_barrier;

	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = nullptr;
	image_memory_barrier.srcAccessMask = source_access_mask;
	image_memory_barrier.dstAccessMask = destination_access_mask;
	image_memory_barrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

	image_memory_barrier.oldLayout = old_layout;
	image_memory_barrier.newLayout = new_layout;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.image = image;

	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = 1;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;
	image_memory_barrier.subresourceRange.layerCount = 1;

	//Record the pipeline barrier command.
	vkCmdPipelineBarrier(_VulkanCommandBuffer, source_stage_mask, destination_stage_mask, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
}

/*
*	Records a next subpass command.
*/
void VulkanCommandBuffer::CommandNextSubpass() NOEXCEPT
{
	vkCmdNextSubpass(_VulkanCommandBuffer, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
}

/*
*	Records a push constants command.
*/
void VulkanCommandBuffer::CommandPushConstants(const VkPipelineLayout layout, const VkShaderStageFlags stageFlags, const uint32 offset, const uint32 size, const void *RESTRICT data) NOEXCEPT
{
	vkCmdPushConstants(_VulkanCommandBuffer, layout, stageFlags, offset, size, data);
}

/*
*	Records a reset event command.
*/
void VulkanCommandBuffer::CommandResetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT
{
	vkCmdResetEvent(_VulkanCommandBuffer, event, stageMask);
}

/*
*	Records a reset query pool command.
*/
void VulkanCommandBuffer::CommandResetQueryPool(const VkQueryPool query_pool, const uint32 query_count) NOEXCEPT
{
	vkCmdResetQueryPool(_VulkanCommandBuffer, query_pool, 0, query_count);
}

/*
*	Records a set event command.
*/
void VulkanCommandBuffer::CommandSetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT
{
	vkCmdSetEvent(_VulkanCommandBuffer, event, stageMask);
}

/*
*	Records a trace rays command.
*/
void VulkanCommandBuffer::CommandTraceRays(	const VkBuffer shader_binding_table_buffer,
											const uint32 miss_shader_binding_offset,
											const uint32 miss_shader_binding_stride,
											const uint32 hit_shader_binding_offset,
											const uint32 hit_shader_binding_stride,
											const uint32 width,
											const uint32 height) NOEXCEPT
{
	vkCmdTraceRaysNV(	_VulkanCommandBuffer,
						shader_binding_table_buffer,
						0,
						shader_binding_table_buffer,
						miss_shader_binding_offset,
						miss_shader_binding_stride,
						shader_binding_table_buffer,
						hit_shader_binding_offset,
						hit_shader_binding_stride,
						VK_NULL_HANDLE,
						0,
						0,
						width,
						height,
						1);
}

/*
*	Records a wait events command.
*/
void VulkanCommandBuffer::CommandWaitEvents(const uint32 eventCount, const VkEvent *const RESTRICT events, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask) NOEXCEPT
{
	vkCmdWaitEvents(_VulkanCommandBuffer, eventCount, events, sourceStageMask, destinationStageMask, 0, nullptr, 0, nullptr, 0, nullptr);
}

/*
*	Records a write timestamp command.
*/
void VulkanCommandBuffer::CommandWriteTimestamp(const VkPipelineStageFlagBits pipeline_stage, const VkQueryPool query_pool, const uint32  query_index) NOEXCEPT
{
	vkCmdWriteTimestamp(_VulkanCommandBuffer, pipeline_stage, query_pool, query_index);
}

/*
*	Ends this Vulkan command buffer.
*/
void VulkanCommandBuffer::End() NOEXCEPT
{
	//End this Vulkan command buffer.
	VULKAN_ERROR_CHECK(vkEndCommandBuffer(_VulkanCommandBuffer));
}

/*
*	Resets this Vulkan command buffer.
*/
void VulkanCommandBuffer::Reset() NOEXCEPT
{
	VULKAN_ERROR_CHECK(vkResetCommandBuffer(_VulkanCommandBuffer, 0));
}

/*
*	Creates a command buffer allocate info.
*/
void VulkanCommandBuffer::CreateCommandBufferAllocateInfo(VkCommandBufferAllocateInfo &commandBufferAllocateInfo, const VulkanCommandPool &vulkanCommandPool, const VkCommandBufferLevel level) const NOEXCEPT
{
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = vulkanCommandPool.Get();
	commandBufferAllocateInfo.level = level;
	commandBufferAllocateInfo.commandBufferCount = 1;
}

/*
*	Creates a primary command buffer begin info.
*/
void VulkanCommandBuffer::CreatePrimaryCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags) const NOEXCEPT
{
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = commandBufferUsageFlags;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
}

/*
*	Creates a command buffer inheritance info.
*/
void VulkanCommandBuffer::CreateCommandBufferInheritanceInfo(VkCommandBufferInheritanceInfo &commandBufferInheritanceInfo, const VkRenderPass renderPass, const uint32 subpass, const VkFramebuffer framebuffer) const NOEXCEPT
{
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = renderPass;
	commandBufferInheritanceInfo.subpass = subpass;
	commandBufferInheritanceInfo.framebuffer = framebuffer;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	commandBufferInheritanceInfo.queryFlags = 0;
	commandBufferInheritanceInfo.pipelineStatistics = 0;
}

/*
*	Creates a secondary command buffer begin info.
*/
void VulkanCommandBuffer::CreateSecondaryCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags, const VkCommandBufferInheritanceInfo *const RESTRICT inheritanceInfo) const NOEXCEPT
{
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = commandBufferUsageFlags;
	commandBufferBeginInfo.pInheritanceInfo = inheritanceInfo;
}
#endif