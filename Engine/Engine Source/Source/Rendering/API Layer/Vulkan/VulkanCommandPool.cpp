//Header file.
#include <Rendering/API Layer/Vulkan/VulkanCommandPool.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>
#include <Rendering/API Layer/Vulkan/VulkanLogicalDevice.h>

/*
*	Initializes this Vulkan command pool.
*/
void VulkanCommandPool::Initialize(const VkCommandPoolCreateFlags flags, const uint32 queueFamilyIndex) NOEXCEPT
{
	//Create the command pool create info.
	VkCommandPoolCreateInfo commandPoolCreateInfo;
	CreateCommandPoolCreateInfo(commandPoolCreateInfo, flags, queueFamilyIndex);

	//Create the command pool!
	VULKAN_ERROR_CHECK(vkCreateCommandPool(VulkanInterface::Instance->GetLogicalDevice().Get(), &commandPoolCreateInfo, nullptr, &vulkanCommandPool));
}

/*
*	Releases this Vulkan command pool.
*/
void VulkanCommandPool::Release() NOEXCEPT
{
	//Destroy the Vulkan command pool.
	vkDestroyCommandPool(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanCommandPool, nullptr);
}

/*
*	Allocates and returns a primary command buffer.
*/
void VulkanCommandPool::AllocatePrimaryCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT
{
	//Initialize the primary command buffer.
	vulkanCommandBuffer.Initialize(*this, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}

/*
*	Allocates and returns a secondary command buffer.
*/
void VulkanCommandPool::AllocateSecondaryCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT
{
	//Initialize the secondary command buffer.
	vulkanCommandBuffer.Initialize(*this, VK_COMMAND_BUFFER_LEVEL_SECONDARY);
}

/*
*	Frees a command buffer.
*/
void VulkanCommandPool::FreeCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT
{
	//Free the command buffer.
	vkFreeCommandBuffers(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanCommandPool, 1, &vulkanCommandBuffer.Get());
}

/*
*	Creates a command pool create info.
*/
void VulkanCommandPool::CreateCommandPoolCreateInfo(VkCommandPoolCreateInfo &commandPoolCreateInfo, const VkCommandPoolCreateFlags flags, const uint32 queueFamilyIndex) const NOEXCEPT
{
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.flags = flags;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
}