#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanPhysicalDevice final
{

public:

	/*
	*	Returns the underlying Vulkan physical device object.
	*/
	const VkPhysicalDevice& Get() const NOEXCEPT { return vulkanPhysicalDevice; }

	/*
	*	Initializes this Vulkan physical device.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Returns the physical device features.
	*/
	const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const NOEXCEPT { return physicalDeviceFeatures; }

	/*
	*	Returns the physical device memory properties.
	*/
	const VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() const NOEXCEPT { return physicalDeviceMemoryProperties; }

	/*
	*	Returns the physical device properties.
	*/
	const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const NOEXCEPT { return physicalDeviceProperties; }

	/*
	*	Returns the surface format.
	*/
	const VkPresentModeKHR& GetPresentMode() const NOEXCEPT { return presentMode; }

	/*
	*	Returns the surface capabilities.
	*/
	const VkSurfaceCapabilitiesKHR& GetSurfaceCapabilities() const NOEXCEPT { return surfaceCapabilities; }

	/*
	*	Returns the surface format.
	*/
	const VkSurfaceFormatKHR& GetSurfaceFormat() const NOEXCEPT { return surfaceFormat; }

private:

	//The underlying Vulkan physical device.
	VkPhysicalDevice vulkanPhysicalDevice;

	//The physical device features.
	VkPhysicalDeviceFeatures physicalDeviceFeatures;

	//The physical device memory properties.
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	//The physical device properties.
	VkPhysicalDeviceProperties physicalDeviceProperties;

	//The present mode.
	VkPresentModeKHR presentMode;

	//The surface capabilities.
	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	//The surface format.
	VkSurfaceFormatKHR surfaceFormat;

	/*
	*	Given a Vulkan physical device, return if it is suitable for this game.
	*/
	bool IsPhysicalDeviceSuitable(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT;

	/*
	*	Given a Vulkan physical device, return if it has the required features.
	*/
	bool HasRequiredFeatures(const VkPhysicalDevice vulkanPhysicalDevice) const NOEXCEPT;

	/*
	*	Given a Vulkan physical device, return if it has the required extensions.
	*/
	bool HasRequiredExtensions(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT;

	/*
	*	Given a Vulkan physical device and a Vulkan surface, return if the Physical device has the proper swap chain support.
	*/
	bool HasProperSwapChainSupport(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT;

	/*
	*	Given a list of suitable physical devices, return most suitable physical device.
	*/
	VkPhysicalDevice GetMostSuitableDevice(const DynamicArray<VkPhysicalDevice> &suitablePhysicalDevices) const NOEXCEPT;

	/*
	*	Given a physical device and a surface, returns the most optimal surface format.
	*/
	VkSurfaceFormatKHR GetMostOptimalSurfaceFormat() const NOEXCEPT;

	/*
	*	Given a physical device and a surface, returns the most optimal present mode.
	*/
	VkPresentModeKHR GetMostOptimalPresentMode() const NOEXCEPT;

};