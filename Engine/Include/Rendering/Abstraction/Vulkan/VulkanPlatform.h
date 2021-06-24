#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class VulkanPlatform final
{

public:

	/*
	*	Callback for when the logical device has been created.
	*/
	static void OnLogicalDeviceCreated() NOEXCEPT;

	/*
	*	Terminates Vulkan for the current platform.
	*/
	static void Terminate() NOEXCEPT;

};