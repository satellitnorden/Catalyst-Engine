#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanSwapChain.h>

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
	*	Initializes the swapchain.
	*/
	static void InitializeSwapchain(VulkanSwapchain *const RESTRICT swapchain) NOEXCEPT;

	/*
	*	Callback for when a frame is started.
	*/
	static void BeginFrame() NOEXCEPT;

	/*
	*	Releases the swapchain.
	*/
	static void ReleaseSwapchain(VulkanSwapchain *const RESTRICT swapchain) NOEXCEPT;

	/*
	*	Terminates Vulkan for the current platform.
	*/
	static void Terminate() NOEXCEPT;

};