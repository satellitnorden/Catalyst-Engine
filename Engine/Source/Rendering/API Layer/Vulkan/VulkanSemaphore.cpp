//Header file.
#include <Rendering/API Layer/Vulkan/VulkanSemaphore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanSemaphore::VulkanSemaphore() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanSemaphore::~VulkanSemaphore() NOEXCEPT
{

}

/*
*	Initializes this Vulkan semaphore.
*/
void VulkanSemaphore::Initialize() NOEXCEPT
{
	//Create the semaphore create info.
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	CreateSemaphoreCreateInfo(semaphoreCreateInfo);

	//Create the Vulkan semaphore!
	VULKAN_ERROR_CHECK(vkCreateSemaphore(VulkanInterface::Instance->GetLogicalDevice().Get(), &semaphoreCreateInfo, nullptr, &vulkanSemaphore));
}

/*
*	Releases this Vulkan semaphore.
*/
void VulkanSemaphore::Release() NOEXCEPT
{
	//Destroy the Vulkan semaphore.
	vkDestroySemaphore(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanSemaphore, nullptr);
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