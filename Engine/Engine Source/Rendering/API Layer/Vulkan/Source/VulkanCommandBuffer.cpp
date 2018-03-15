//Header file.
#include <VulkanCommandBuffer.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanCommandBuffer::VulkanCommandBuffer() NOEXCEPT
{
	
}

/*
*	Default destructor.
*/
VulkanCommandBuffer::~VulkanCommandBuffer() NOEXCEPT
{

}

/*
*	Initializes this Vulkan command buffer.
*/
void VulkanCommandBuffer::Initialize(const VulkanCommandPool &vulkanCommandPool) NOEXCEPT
{
	//Create the command buffer allocate info.
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	CreateCommandBufferAllocateInfo(commandBufferAllocateInfo, vulkanCommandPool);

	//Allocate the command buffer!
	VULKAN_ERROR_CHECK(vkAllocateCommandBuffers(VulkanInterface::Instance->GetLogicalDevice().Get(), &commandBufferAllocateInfo, &vulkanCommandBuffer));
}

/*
*	Begins this Vulkan command buffer.
*/
void VulkanCommandBuffer::Begin(const VkCommandBufferUsageFlags commandBufferUsageFlags) NOEXCEPT
{
	//Create the command buffer begin info.
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	CreateCommandBufferBeginInfo(commandBufferBeginInfo, commandBufferUsageFlags);

	//Begin the command buffer!
	VULKAN_ERROR_CHECK(vkBeginCommandBuffer(vulkanCommandBuffer, &commandBufferBeginInfo));
}

/*
*	Records a begin render pass command.
*/
void VulkanCommandBuffer::CommandBeginRenderPass(const VulkanRenderPass &vulkanRenderPass, const uint64 framebufferIndex) NOEXCEPT
{
	VkRenderPassBeginInfo renderPassBeginInfo;

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vulkanRenderPass.Get();
	renderPassBeginInfo.framebuffer = vulkanRenderPass.GetFrameBuffers()[framebufferIndex].Get();
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
	renderPassBeginInfo.clearValueCount = 0;
	renderPassBeginInfo.pClearValues = nullptr;

	vkCmdBeginRenderPass(vulkanCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

/*
*	Records a bind descriptor sets command.
*/
void VulkanCommandBuffer::CommandBindDescriptorSets(const VulkanPipeline &vulkanPipeline, const uint32 descriptorSetCount, const VulkanDescriptorSet *RESTRICT descriptorSets) NOEXCEPT
{
	vkCmdBindDescriptorSets(vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.GetPipelineLayout(), 0, descriptorSetCount, ReinterpretCast<const VkDescriptorSet *RESTRICT>(descriptorSets), 0, nullptr);
}

/*
*	Records a bind index buffer command.
*/
void VulkanCommandBuffer::CommandBindIndexBuffer(const VulkanBuffer &vulkanIndexBuffer, const VkDeviceSize offset) NOEXCEPT
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
void VulkanCommandBuffer::CommandDraw(const uint32 vertexCount) NOEXCEPT
{
	vkCmdDraw(vulkanCommandBuffer, vertexCount, 1, 0, 0);
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
*	Records a push constants command.
*/
void VulkanCommandBuffer::CommandPushConstants(const VkPipelineLayout layout, const VkShaderStageFlags stageFlags, const uint32 offset, const uint32 size, const void *RESTRICT data) NOEXCEPT
{
	vkCmdPushConstants(vulkanCommandBuffer, layout, stageFlags, offset, size, data);
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
void VulkanCommandBuffer::CreateCommandBufferAllocateInfo(VkCommandBufferAllocateInfo &commandBufferAllocateInfo, const VulkanCommandPool &vulkanCommandPool) const NOEXCEPT
{
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = vulkanCommandPool.Get();
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;
}

/*
*	Creates a command buffer begin info.
*/
void VulkanCommandBuffer::CreateCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags) const NOEXCEPT
{
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = commandBufferUsageFlags;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
}