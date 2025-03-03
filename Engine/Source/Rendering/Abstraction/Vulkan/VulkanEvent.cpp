#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanEvent.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan event.
*/
void VulkanEvent::Initialize() NOEXCEPT
{
	//Create the event create info.
	VkEventCreateInfo eventCreateInfo;
	CreateEventCreateInfo(eventCreateInfo);

	//Create the event!
	VULKAN_ERROR_CHECK(vkCreateEvent(VulkanInterface::Instance->GetLogicalDevice().Get(), &eventCreateInfo, nullptr, &_VulkanEvent));
}

/*
*	Releases this Vulkan event.
*/
void VulkanEvent::Release() NOEXCEPT
{
	//Destroy the event!
	vkDestroyEvent(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanEvent, nullptr);
}

/*
*	Resets this Vulkan event.
*/
void VulkanEvent::Reset() NOEXCEPT
{
	//Reset the event!
	VULKAN_ERROR_CHECK(vkResetEvent(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanEvent));
}

/*
*	Sets this Vulkan event.
*/
void VulkanEvent::Set() NOEXCEPT
{
	//Set the event!
	VULKAN_ERROR_CHECK(vkSetEvent(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanEvent));
}

/*
*	Waits for this vulkan event.
*/
void VulkanEvent::WaitFor() NOEXCEPT
{
	while (vkGetEventStatus(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanEvent) == VK_EVENT_RESET);
}

/*
*	Creates an event create info.
*/
void VulkanEvent::CreateEventCreateInfo(VkEventCreateInfo &eventCreateInfo) const NOEXCEPT
{
	eventCreateInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
	eventCreateInfo.pNext = nullptr;
	eventCreateInfo.flags = 0;
}
#endif