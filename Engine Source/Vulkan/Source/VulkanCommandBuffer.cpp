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
void VulkanCommandBuffer::CommandBeginRenderPass(const VulkanRenderPass &vulkanRenderPass, const size_t framebufferIndex, const bool clearDepth, const uint32 numberOfClearValues) NOEXCEPT
{
	DynamicArray<VkClearValue> clearValues;

	for (uint32 i = 0; i < numberOfClearValues; ++i)
	{
		VkClearValue newClearValue;

		if (clearDepth && i == 0)
		{
			newClearValue.color = { 0.0f, 0.0f, 0.0f, 0.0f };
			newClearValue.depthStencil = { 1.0f, 0 };
		}
		
		else
		{
			newClearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			newClearValue.depthStencil = { 0.0f, 0 };
		}

		clearValues.Emplace(newClearValue);
	}

	VkRenderPassBeginInfo renderPassBeginInfo;

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vulkanRenderPass.Get();
	renderPassBeginInfo.framebuffer = vulkanRenderPass.GetFrameBuffers()[framebufferIndex].Get();
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = VulkanInterface::Instance->GetSwapchain().GetSwapExtent();
	renderPassBeginInfo.clearValueCount = static_cast<uint32>(clearValues.Size());
	renderPassBeginInfo.pClearValues = clearValues.Data();

	vkCmdBeginRenderPass(vulkanCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

/*
*	Records a bind descriptor sets command.
*/
void VulkanCommandBuffer::CommandBindDescriptorSets(const VulkanPipeline &vulkanPipeline, const VulkanDescriptorSet &vulkanDescriptorSet) NOEXCEPT
{
	vkCmdBindDescriptorSets(vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.GetPipelineLayout(), 0, 1, &vulkanDescriptorSet.Get(), 0, nullptr);
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
void VulkanCommandBuffer::CommandBindVertexBuffers(const VulkanBuffer &vulkanVertexBuffer, const VkDeviceSize offset) NOEXCEPT
{
	vkCmdBindVertexBuffers(vulkanCommandBuffer, 0, 1, &vulkanVertexBuffer.Get(), &offset);
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
void VulkanCommandBuffer::CommandDrawIndexed(const uint32 indexCount) NOEXCEPT
{
	vkCmdDrawIndexed(vulkanCommandBuffer, indexCount, 1, 0, 0, 0);
}

/*
*	Records an end render pass command.
*/
void VulkanCommandBuffer::CommandEndRenderPass() NOEXCEPT
{
	vkCmdEndRenderPass(vulkanCommandBuffer);
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