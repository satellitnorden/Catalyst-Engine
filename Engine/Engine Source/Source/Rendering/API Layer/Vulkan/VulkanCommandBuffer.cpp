//Header file.
#include <Rendering/API Layer/Vulkan/VulkanCommandBuffer.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan command buffer.
*/
void VulkanCommandBuffer::Initialize(const VulkanCommandPool &vulkanCommandPool, const VkCommandBufferLevel level) NOEXCEPT
{
	//Create the command buffer allocate info.
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	CreateCommandBufferAllocateInfo(commandBufferAllocateInfo, vulkanCommandPool, level);

	//Allocate the command buffer!
	VULKAN_ERROR_CHECK(vkAllocateCommandBuffers(VulkanInterface::Instance->GetLogicalDevice().Get(), &commandBufferAllocateInfo, &vulkanCommandBuffer));
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
	VULKAN_ERROR_CHECK(vkBeginCommandBuffer(vulkanCommandBuffer, &commandBufferBeginInfo));
}

/*
*	Begins this Vulkan command buffer as a secondary command buffer.
*/
void VulkanCommandBuffer::BeginSecondary(const VkCommandBufferUsageFlags commandBufferUsageFlags, const VkRenderPass renderPass, const VkFramebuffer framebuffer) NOEXCEPT
{
	//Create the command buffer inheritance info.
	VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
	CreateCommandBufferInheritanceInfo(commandBufferInheritanceInfo, renderPass, framebuffer);

	//Create the command buffer begin info.
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	CreateSecondaryCommandBufferBeginInfo(commandBufferBeginInfo, commandBufferUsageFlags, &commandBufferInheritanceInfo);

	//Begin the command buffer!
	VULKAN_ERROR_CHECK(vkBeginCommandBuffer(vulkanCommandBuffer, &commandBufferBeginInfo));
}

/*
*	Records a begin render pass command.
*/
void VulkanCommandBuffer::CommandBeginRenderPass(const VulkanRenderPass &vulkanRenderPass, const uint64 framebufferIndex, const VkExtent2D renderArea, const VkSubpassContents contents) NOEXCEPT
{
	VkRenderPassBeginInfo renderPassBeginInfo;

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vulkanRenderPass.Get();
	renderPassBeginInfo.framebuffer = vulkanRenderPass.GetFrameBuffers()[framebufferIndex].Get();
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = renderArea;
	renderPassBeginInfo.clearValueCount = 0;
	renderPassBeginInfo.pClearValues = nullptr;

	vkCmdBeginRenderPass(vulkanCommandBuffer, &renderPassBeginInfo, contents);
}

/*
*	Records a bind descriptor sets command.
*/
void VulkanCommandBuffer::CommandBindDescriptorSets(const VulkanPipeline &vulkanPipeline, const uint32 descriptorSetCount, const VulkanDescriptorSet *RESTRICT descriptorSets) NOEXCEPT
{
	vkCmdBindDescriptorSets(vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.GetPipelineLayout(), 0, descriptorSetCount, reinterpret_cast<const VkDescriptorSet *RESTRICT>(descriptorSets), 0, nullptr);
}

/*
*	Records a bind index buffer command.
*/
void VulkanCommandBuffer::CommandBindIndexBuffer(const VulkanConstantBuffer &vulkanIndexBuffer, const VkDeviceSize offset) NOEXCEPT
{
	vkCmdBindIndexBuffer(vulkanCommandBuffer, vulkanIndexBuffer.Get(), offset, VK_INDEX_TYPE_UINT32);
}

/*
*	Records a bind pipeline command.
*/
void VulkanCommandBuffer::CommandBindPipeline(const VulkanPipeline &vulkanPipeline) NOEXCEPT
{
	vkCmdBindPipeline(vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.Get());
}

/*
*	Records a bind vertex buffers command.
*/
void VulkanCommandBuffer::CommandBindVertexBuffers(const uint32 vertexBufferCount, const VkBuffer *RESTRICT vertexBuffers, const VkDeviceSize *RESTRICT offsets) NOEXCEPT
{
	vkCmdBindVertexBuffers(vulkanCommandBuffer, 0, vertexBufferCount, vertexBuffers, offsets);
}

/*
*	Records a draw command.
*/
void VulkanCommandBuffer::CommandDraw(const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{
	vkCmdDraw(vulkanCommandBuffer, vertexCount, instanceCount, 0, 0);
}

/*
*	Records a draw indexed command.
*/
void VulkanCommandBuffer::CommandDrawIndexed(const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	vkCmdDrawIndexed(vulkanCommandBuffer, indexCount, instanceCount, 0, 0, 0);
}

/*
*	Records an end render pass command.
*/
void VulkanCommandBuffer::CommandEndRenderPass() NOEXCEPT
{
	vkCmdEndRenderPass(vulkanCommandBuffer);
}

/*
*	Records an execute commands command.
*/
void VulkanCommandBuffer::CommandExecuteCommands(const VkCommandBuffer commandBuffer) NOEXCEPT
{
	vkCmdExecuteCommands(vulkanCommandBuffer, 1, &commandBuffer);
}

/*
*	Records a push constants command.
*/
void VulkanCommandBuffer::CommandPushConstants(const VkPipelineLayout layout, const VkShaderStageFlags stageFlags, const uint32 offset, const uint32 size, const void *RESTRICT data) NOEXCEPT
{
	vkCmdPushConstants(vulkanCommandBuffer, layout, stageFlags, offset, size, data);
}

/*
*	Records a reset event command.
*/
void VulkanCommandBuffer::CommandResetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT
{
	vkCmdResetEvent(vulkanCommandBuffer, event, stageMask);
}

/*
*	Records a set event command.
*/
void VulkanCommandBuffer::CommandSetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT
{
	vkCmdSetEvent(vulkanCommandBuffer, event, stageMask);
}

/*
*	Records a wait events command.
*/
void VulkanCommandBuffer::CommandWaitEvents(const uint32 eventCount, const VkEvent *const RESTRICT events, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask) NOEXCEPT
{
	vkCmdWaitEvents(vulkanCommandBuffer, eventCount, events, sourceStageMask, destinationStageMask, 0, nullptr, 0, nullptr, 0, nullptr);
}

/*
*	Ends this Vulkan command buffer.
*/
void VulkanCommandBuffer::End() NOEXCEPT
{
	//End this Vulkan command buffer.
	VULKAN_ERROR_CHECK(vkEndCommandBuffer(vulkanCommandBuffer));
}

/*
*	Resets this Vulkan command buffer.
*/
void VulkanCommandBuffer::Reset() NOEXCEPT
{
	VULKAN_ERROR_CHECK(vkResetCommandBuffer(vulkanCommandBuffer, 0));
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
void VulkanCommandBuffer::CreateCommandBufferInheritanceInfo(VkCommandBufferInheritanceInfo &commandBufferInheritanceInfo, const VkRenderPass renderPass, const VkFramebuffer framebuffer) const NOEXCEPT
{
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = renderPass;
	commandBufferInheritanceInfo.subpass = 0;
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