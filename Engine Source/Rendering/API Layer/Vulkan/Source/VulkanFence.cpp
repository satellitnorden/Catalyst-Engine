//Header file.
#include <VulkanFence.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanFence::VulkanFence() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanFence::~VulkanFence() NOEXCEPT
{

}

/*
*	Initializes this Vulkan fence.
*/
void VulkanFence::Initialize(const VkFenceCreateFlags flags) NOEXCEPT
{
	//Create the fence create info.
	VkFenceCreateInfo fenceCreateInfo;
	CreateFenceCreateInfo(fenceCreateInfo, flags);

	//Create the Vulkan fence!
	VULKAN_ERROR_CHECK(vkCreateFence(VulkanInterface::Instance->GetLogicalDevice().Get(), &fenceCreateInfo, nullptr, &vulkanFence));
}

/*
*	Creates a fence create info.
*/
void VulkanFence::CreateFenceCreateInfo(VkFenceCreateInfo &fenceCreateInfo, const VkFenceCreateFlags flags) const NOEXCEPT
{
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = flags;
}

/*
*	Releases this Vulkan fence.
*/
void VulkanFence::Release() NOEXCEPT
{
	//Destroy the Vulkan fence.
	vkDestroyFence(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanFence, nullptr);
}

/*
*	Returns whether or not this fence is signaled.
*/
bool VulkanFence::IsSignaled() const NOEXCEPT
{
	//Return whether or not this fence is signaled.
	return vkGetFenceStatus(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanFence) == VK_SUCCESS;
}

/*
*	Resets this Vulkan fence.
*/
void VulkanFence::Reset() NOEXCEPT
{
	//Reset this Vulkan fence.
	VULKAN_ERROR_CHECK(vkResetFences(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &vulkanFence));
}

/*
*	Waits for this Vulkan fence.
*/
void VulkanFence::WaitFor() const NOEXCEPT
{
	//Wait for this Vulkan fence.
	while (vkWaitForFences(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &vulkanFence, VK_TRUE, 0) == VK_TIMEOUT);
}