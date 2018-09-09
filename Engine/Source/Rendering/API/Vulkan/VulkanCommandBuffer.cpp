//Header file.
#include <Rendering/API/Vulkan/VulkanCommandBuffer.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

//Vulkan command buffer constants.
namespace VulkanCommandBufferConstants
{
	constexpr uint64 MAXIMUM_NUMBER_OF_CLEAR_VALUES{ 6 };
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
void VulkanCommandBuffer::CommandBeginRenderPassAndClear(const VkRenderPass renderPass, const VkFramebuffer framebuffer, const VkExtent2D renderArea, const VkSubpassContents contents, const uint32 numberOfClearValues) NOEXCEPT
{
	StaticArray<VkClearValue, VulkanCommandBufferConstants::MAXIMUM_NUMBER_OF_CLEAR_VALUES> clearValues;

	for (uint32 i = 0; i < numberOfClearValues; ++i)
	{
		clearValues[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };
		clearValues[i].depthStencil = { 1.0f, 0 };
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
void VulkanCommandBuffer::CommandBindDescriptorSets(const VkPipelineLayout pipelineLayout, const uint32 firstBinding, const uint32 descriptorSetCount, const VkDescriptorSet *RESTRICT descriptorSets) NOEXCEPT
{
	vkCmdBindDescriptorSets(_VulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, firstBinding, descriptorSetCount, descriptorSets, 0, nullptr);
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
void VulkanCommandBuffer::CommandBindPipeline(const VkPipeline vulkanPipeline) NOEXCEPT
{
	vkCmdBindPipeline(_VulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline);
}

/*
*	Records a bind vertex buffers command.
*/
void VulkanCommandBuffer::CommandBindVertexBuffers(const uint32 vertexBufferCount, const VkBuffer *RESTRICT vertexBuffers, const VkDeviceSize *RESTRICT offsets) NOEXCEPT
{
	vkCmdBindVertexBuffers(_VulkanCommandBuffer, 0, vertexBufferCount, vertexBuffers, offsets);
}

/*
*	Records a copy image command.
*/
void VulkanCommandBuffer::CommandCopyImage(VkImage source, VkImage destination, const VkExtent3D extent) NOEXCEPT
{
	VkImageCopy imageCopy;

	imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.srcSubresource.mipLevel = 0;
	imageCopy.srcSubresource.baseArrayLayer = 0;
	imageCopy.srcSubresource.layerCount = 1;
	imageCopy.srcOffset = { 0, 0, 0 };
	imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.dstSubresource.mipLevel = 0;
	imageCopy.dstSubresource.baseArrayLayer = 0;
	imageCopy.dstSubresource.layerCount = 1;
	imageCopy.dstOffset = { 0, 0, 0 };
	imageCopy.extent = extent;

	vkCmdCopyImage(_VulkanCommandBuffer, source, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destination, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
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
*	Records a next subpass command.
*/
void VulkanCommandBuffer::CommandNextSubpass() NOEXCEPT
{
	vkCmdNextSubpass(_VulkanCommandBuffer, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
}

/*
*	Records a pipeline barrier command.
*/
void VulkanCommandBuffer::CommandPipelineBarrier(const VkAccessFlags sourceAccessMask, const VkAccessFlags destinationAccessMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, VkImage image, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask, const VkDependencyFlags dependencyFlags) NOEXCEPT
{
	VkImageMemoryBarrier imageMemoryBarrier;

	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = sourceAccessMask;
	imageMemoryBarrier.dstAccessMask = destinationAccessMask;
	imageMemoryBarrier.oldLayout = oldLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(_VulkanCommandBuffer, sourceStageMask, destinationStageMask, dependencyFlags, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
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
*	Records a set event command.
*/
void VulkanCommandBuffer::CommandSetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT
{
	vkCmdSetEvent(_VulkanCommandBuffer, event, stageMask);
}

/*
*	Records a wait events command.
*/
void VulkanCommandBuffer::CommandWaitEvents(const uint32 eventCount, const VkEvent *const RESTRICT events, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask) NOEXCEPT
{
	vkCmdWaitEvents(_VulkanCommandBuffer, eventCount, events, sourceStageMask, destinationStageMask, 0, nullptr, 0, nullptr, 0, nullptr);
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
	commandBufferInheritanceInfo.pipelineStatistics = VK_NULL_HANDLE;
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