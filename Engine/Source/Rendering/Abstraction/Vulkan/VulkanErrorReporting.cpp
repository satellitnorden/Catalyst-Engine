#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanErrorReporting.h>

//Core.
#include <Core/General/DynamicString.h>

//Systems.
#include <Systems/LogSystem.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

#if VULKAN_DEBUGGING
//Static variable definitions.
VkDebugUtilsMessengerEXT VulkanErrorReporting::_Messenger;

/*
*	Initializes the Vulkan error reporting.
*/
void VulkanErrorReporting::Initialize() NOEXCEPT
{
	//Create the messenger.
	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;

	debugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugUtilsMessengerCreateInfoEXT.pNext = nullptr;
	debugUtilsMessengerCreateInfoEXT.flags = 0;
	debugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugUtilsMessengerCreateInfoEXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugUtilsMessengerCreateInfoEXT.pfnUserCallback = ErrorCallback;
	debugUtilsMessengerCreateInfoEXT.pUserData = nullptr;

	auto function{ reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(VulkanInterface::Instance->GetInstance().Get(), "vkCreateDebugUtilsMessengerEXT")) };

	VULKAN_ERROR_CHECK(function(VulkanInterface::Instance->GetInstance().Get(), &debugUtilsMessengerCreateInfoEXT, nullptr, &_Messenger));
}

/*
*	Releases the Vulkan error reporting.
*/
void VulkanErrorReporting::Release() NOEXCEPT
{
	//Destroy messenger.
	auto function{ reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(VulkanInterface::Instance->GetInstance().Get(), "vkDestroyDebugUtilsMessengerEXT")) };

	function(VulkanInterface::Instance->GetInstance().Get(), _Messenger, nullptr);
}

/*
*	The error callback.
*/
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanErrorReporting::ErrorCallback(	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																	VkDebugUtilsMessageTypeFlagsEXT messageType,
																	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
																	void *pUserData)
{
	LOG_ERROR(pCallbackData->pMessage);

#if 0
	{
		DynamicString message{ pCallbackData->pMessage };

		ASSERT(!message.Find("Validation Error:"), "Validation error detected!");
		ASSERT(!message.Find("Validation Performance Warning:"), "Validation performance warning detected!");
	}
#endif

	return VK_FALSE;
}
#endif
#endif