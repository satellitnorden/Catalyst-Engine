#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

#if VULKAN_DEBUGGING
class VulkanErrorReporting final
{

public:

	/*
	*	Initializes the Vulkan error reporting.
	*/
	static void Initialize() NOEXCEPT;

	/*
	*	Releases the Vulkan error reporting.
	*/
	static void Release() NOEXCEPT;

private:

	//The messenger.
	static VkDebugUtilsMessengerEXT _Messenger;

	/*
	*	The error callback.
	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL ErrorCallback(	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
															VkDebugUtilsMessageTypeFlagsEXT messageType,
															const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
															void* pUserData);

};
#endif