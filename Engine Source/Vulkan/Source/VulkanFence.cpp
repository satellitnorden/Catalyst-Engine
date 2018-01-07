//Header file.
#include <VulkanFence.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanFence::VulkanFence() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanFence::~VulkanFence() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan fence.
*/
void VulkanFence::Initialize() CATALYST_NOEXCEPT
{
	//Create the fence create info.
	VkFenceCreateInfo fenceCreateInfo;
	CreateFenceCreateInfo(fenceCreateInfo);

	//Create the Vulkan fence!
	VULKAN_ERROR_CHECK(vkCreateFence(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &fenceCreateInfo, nullptr, &vulkanFence));
}

/*
*	Creates a fence create info.
*/
void VulkanFence::CreateFenceCreateInfo(VkFenceCreateInfo &fenceCreateInfo) const CATALYST_NOEXCEPT
{
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = 0;
}

/*
*	Releases this Vulkan fence.
*/
void VulkanFence::Release() CATALYST_NOEXCEPT
{
	//Destroy the Vulkan fence.
	vkDestroyFence(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanFence, nullptr);
}

/*
*	Resets this Vulkan fence.
*/
void VulkanFence::Reset() CATALYST_NOEXCEPT
{
	//Reset this Vulkan fence.
	vkResetFences(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), 1, &vulkanFence);
}

/*
*	Waits for this Vulkan fence.
*/
void VulkanFence::WaitFor() const CATALYST_NOEXCEPT
{
	//Wait for this Vulkan fence.
	vkWaitForFences(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), 1, &vulkanFence, VK_TRUE, UINT64_MAXIMUM);
}