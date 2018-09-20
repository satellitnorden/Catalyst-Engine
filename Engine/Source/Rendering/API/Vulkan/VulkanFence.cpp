//Header file.
#include <Rendering/API/Vulkan/VulkanFence.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan fence.
*/
void VulkanFence::Initialize(const VkFenceCreateFlags flags) NOEXCEPT
{
	//Create the fence create info.
	VkFenceCreateInfo fenceCreateInfo;
	CreateFenceCreateInfo(fenceCreateInfo, flags);

	//Create the Vulkan fence!
	VULKAN_ERROR_CHECK(vkCreateFence(VulkanInterface::Instance->GetLogicalDevice().Get(), &fenceCreateInfo, nullptr, &_VulkanFence));
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
	vkDestroyFence(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanFence, nullptr);
}

/*
*	Returns whether or not this fence is signaled.
*/
bool VulkanFence::IsSignaled() const NOEXCEPT
{
	//Return whether or not this fence is signaled.
	return vkGetFenceStatus(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanFence) == VK_SUCCESS;
}

/*
*	Resets this Vulkan fence.
*/
void VulkanFence::Reset() NOEXCEPT
{
	//Reset this Vulkan fence.
	VULKAN_ERROR_CHECK(vkResetFences(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &_VulkanFence));
}

/*
*	Waits for this Vulkan fence.
*/
void VulkanFence::WaitFor() const NOEXCEPT
{
	//Wait for this Vulkan fence.
	while (vkWaitForFences(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &_VulkanFence, VK_TRUE, 0) == VK_TIMEOUT);
}