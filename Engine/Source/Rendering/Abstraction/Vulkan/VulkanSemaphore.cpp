#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanSemaphore.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan semaphore.
*/
void VulkanSemaphore::Initialize() NOEXCEPT
{
	//Create the semaphore create info.
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	CreateSemaphoreCreateInfo(semaphoreCreateInfo);

	//Create the Vulkan semaphore!
	VULKAN_ERROR_CHECK(vkCreateSemaphore(VulkanInterface::Instance->GetLogicalDevice().Get(), &semaphoreCreateInfo, nullptr, &_VulkanSemaphore));
}

/*
*	Releases this Vulkan semaphore.
*/
void VulkanSemaphore::Release() NOEXCEPT
{
	//Destroy the Vulkan semaphore.
	vkDestroySemaphore(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanSemaphore, nullptr);
}

/*
*	Creates a semaphore create info.
*/
void VulkanSemaphore::CreateSemaphoreCreateInfo(VkSemaphoreCreateInfo &semaphoreCreateInfo) const NOEXCEPT
{
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;
}
#endif