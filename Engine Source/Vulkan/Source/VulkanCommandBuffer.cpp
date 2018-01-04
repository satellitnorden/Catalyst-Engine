//Header file.
#include <VulkanCommandBuffer.h>

//Vulkan.
#include <VulkanCommandPool.h>
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>

/*
*	Default constructor.
*/
VulkanCommandBuffer::VulkanCommandBuffer() CATALYST_NOEXCEPT
{
	
}

/*
*	Default destructor.
*/
VulkanCommandBuffer::~VulkanCommandBuffer() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan command buffer.
*/
void VulkanCommandBuffer::Initialize(const VulkanCommandPool &vulkanCommandPool) CATALYST_NOEXCEPT
{
	//Create the command buffer allocate info.
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	CreateCommandBufferAllocateInfo(commandBufferAllocateInfo, vulkanCommandPool);

	//Allocate the command buffer!
	VkResult result = vkAllocateCommandBuffers(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &commandBufferAllocateInfo, &vulkanCommandBuffer);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif
}

/*
*	Begins this Vulkan command buffer.
*/
void VulkanCommandBuffer::Begin(const VkCommandBufferUsageFlags commandBufferUsageFlags) CATALYST_NOEXCEPT
{
	//Create the command buffer begin info.
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	CreateCommandBufferBeginInfo(commandBufferBeginInfo, commandBufferUsageFlags);

	//Begin the command buffer!
	vkBeginCommandBuffer(vulkanCommandBuffer, &commandBufferBeginInfo);
}

/*
*	Records a begin render pass command.
*/
void VulkanCommandBuffer::CommandBeginRenderPass(const VulkanRenderPass &vulkanRenderPass, const size_t framebufferIndex) CATALYST_NOEXCEPT
{
	DynamicArray<VkClearValue> clearValues;

	clearValues.Reserve(2);

	VkClearValue depthClearValue;

	depthClearValue.color = { 0.0f, 0.0f, 0.0f, 0.0f };
	depthClearValue.depthStencil = { 1.0f, 0 };

	clearValues.EmplaceUnsafe(depthClearValue);

	VkClearValue colorClearValue;

	colorClearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
	colorClearValue.depthStencil = { 0.0f, 0 };

	clearValues.EmplaceUnsafe(colorClearValue);

	VkRenderPassBeginInfo renderPassBeginInfo;

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vulkanRenderPass.Get();
	renderPassBeginInfo.framebuffer = vulkanRenderPass.GetFrameBuffers()[framebufferIndex].Get();
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = VulkanInterface::Instance->GetVulkanSwapchain().GetSwapExtent();
	renderPassBeginInfo.clearValueCount = static_cast<uint32>(clearValues.Size());
	renderPassBeginInfo.pClearValues = clearValues.Data();

	vkCmdBeginRenderPass(vulkanCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

/*
*	Records a bind descriptor sets command.
*/
void VulkanCommandBuffer::CommandBindDescriptorSets(const VulkanPipeline &vulkanPipeline, const VulkanDescriptorSet &vulkanDescriptorSet) CATALYST_NOEXCEPT
{
	vkCmdBindDescriptorSets(vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.GetPipelineLayout(), 0, 1, &vulkanDescriptorSet.Get(), 0, nullptr);
}

/*
*	Records a bind index buffer command.
*/
void VulkanCommandBuffer::CommandBindIndexBuffer(const VulkanIndexBuffer &vulkanIndexBuffer) CATALYST_NOEXCEPT
{
	vkCmdBindIndexBuffer(vulkanCommandBuffer, vulkanIndexBuffer.Get(), 0, VK_INDEX_TYPE_UINT32);
}

/*
*	Records a bind pipeline command.
*/
void VulkanCommandBuffer::CommandBindPipeline(const VulkanPipeline &vulkanPipeline) CATALYST_NOEXCEPT
{
	vkCmdBindPipeline(vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.Get());
}

/*
*	Records a bind vertex buffers command.
*/
void VulkanCommandBuffer::CommandBindVertexBuffers(const VulkanVertexBuffer &vulkanVertexBuffer) CATALYST_NOEXCEPT
{
	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(vulkanCommandBuffer, 0, 1, &vulkanVertexBuffer.Get(), &offset);
}

/*
*	Records a draw indexed command.
*/
void VulkanCommandBuffer::CommandDrawIndexed(const uint32 indexCount) CATALYST_NOEXCEPT
{
	vkCmdDrawIndexed(vulkanCommandBuffer, indexCount, 1, 0, 0, 0);
}

/*
*	Records an end render pass command.
*/
void VulkanCommandBuffer::CommandEndRenderPass() CATALYST_NOEXCEPT
{
	vkCmdEndRenderPass(vulkanCommandBuffer);
}

/*
*	Ends this Vulkan command buffer.
*/
void VulkanCommandBuffer::End() CATALYST_NOEXCEPT
{
	//End this Vulkan command buffer.
	VkResult result = vkEndCommandBuffer(vulkanCommandBuffer);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif
}

/*
*	Resets this Vulkan command buffer.
*/
void VulkanCommandBuffer::Reset() CATALYST_NOEXCEPT
{
	vkResetCommandBuffer(vulkanCommandBuffer, 0);
}

/*
*	Creates a command buffer allocate info.
*/
void VulkanCommandBuffer::CreateCommandBufferAllocateInfo(VkCommandBufferAllocateInfo &commandBufferAllocateInfo, const VulkanCommandPool &vulkanCommandPool) const CATALYST_NOEXCEPT
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
void VulkanCommandBuffer::CreateCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags) const CATALYST_NOEXCEPT
{
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = commandBufferUsageFlags;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
}