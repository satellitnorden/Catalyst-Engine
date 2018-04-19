//Header file.
#include <Rendering/API Layer/Vulkan/VulkanCommandPool.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>
#include <Rendering/API Layer/Vulkan/VulkanLogicalDevice.h>

/*
*	Default constructor.
*/
VulkanCommandPool::VulkanCommandPool() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanCommandPool::~VulkanCommandPool() NOEXCEPT
{

}

/*
*	Initializes this Vulkan command pool.
*/
void VulkanCommandPool::Initialize(const uint32 queueFamilyIndex) NOEXCEPT
{
	//Create the command pool create info.
	VkCommandPoolCreateInfo commandPoolCreateInfo;
	CreateCommandPoolCreateInfo(commandPoolCreateInfo, queueFamilyIndex);

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
*	Allocates and returns a Vulkan command buffer.
*/
void VulkanCommandPool::AllocateVulkanCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT
{
	//Initialize the Vulkan command buffer.
	vulkanCommandBuffer.Initialize(*this);
}

/*
*	Frees a Vulkan command buffer.
*/
void VulkanCommandPool::FreeVulkanCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT
{
	//Free the Vulkan command buffer.
	vkFreeCommandBuffers(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanCommandPool, 1, &vulkanCommandBuffer.Get());
}

/*
*	Creates a command pool create info.
*/
void VulkanCommandPool::CreateCommandPoolCreateInfo(VkCommandPoolCreateInfo &commandPoolCreateInfo, const uint32 queueFamilyIndex) const NOEXCEPT
{
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
}