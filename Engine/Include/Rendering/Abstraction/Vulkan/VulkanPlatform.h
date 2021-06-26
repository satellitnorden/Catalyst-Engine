#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class VulkanPlatform final
{

public:

	/*
	*	Returns the required instance extensions.
	*/
	static void RequiredInstanceExtensions(DynamicArray<const char *const RESTRICT> *const RESTRICT output) NOEXCEPT;

	/*
	*	Returns the required logical device extensions.
	*/
	static void RequiredLogicalDeviceExtensions(DynamicArray<const char *const RESTRICT> *const RESTRICT output) NOEXCEPT;

	/*
	*	Callback for when the logical device has been created.
	*/
	static void OnLogicalDeviceCreated() NOEXCEPT;

	/*
	*	Terminates Vulkan for the current platform.
	*/
	static void Terminate() NOEXCEPT;

};