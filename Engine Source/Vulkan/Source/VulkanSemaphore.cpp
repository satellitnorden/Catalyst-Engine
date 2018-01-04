//Header file.
#include <VulkanSemaphore.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanSemaphore::VulkanSemaphore() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanSemaphore::~VulkanSemaphore() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan semaphore.
*/
void VulkanSemaphore::Initialize() CATALYST_NOEXCEPT
{
	//Create the semaphore create info.
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	CreateSemaphoreCreateInfo(semaphoreCreateInfo);

	//Create the Vulkan semaphore!
	VkResult result = vkCreateSemaphore(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &semaphoreCreateInfo, nullptr, &vulkanSemaphore);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif
}

/*
*	Releases this Vulkan semaphore.
*/
void VulkanSemaphore::Release() CATALYST_NOEXCEPT
{
	//Destroy the Vulkan semaphore.
	vkDestroySemaphore(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanSemaphore, nullptr);
}

/*
*	Creates a semaphore create info.
*/
void VulkanSemaphore::CreateSemaphoreCreateInfo(VkSemaphoreCreateInfo &semaphoreCreateInfo) const CATALYST_NOEXCEPT
{
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;
}