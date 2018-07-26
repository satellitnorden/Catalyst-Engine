#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

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
	const VULKAN_PRESENT_MODE_TYPE& GetPresentMode() const NOEXCEPT { return presentMode; }

	/*
	*	Returns the surface capabilities.
	*/
	const VULKAN_SURFACE_CAPABILITIES_TYPE& GetSurfaceCapabilities() const NOEXCEPT { return surfaceCapabilities; }

	/*
	*	Returns the surface format.
	*/
	const VULKAN_SURFACE_FORMAT_TYPE& GetSurfaceFormat() const NOEXCEPT { return surfaceFormat; }

	/*
	*	Returns the graphics queue family index.
	*/
	uint32 GetGraphicsQueueFamilyIndex() const NOEXCEPT { return graphicsQueueFamilyIndex; }

	/*
	*	Returns the present queue family index.
	*/
	uint32 GetPresentQueueFamilyIndex() const NOEXCEPT { return presentQueueFamilyIndex; }

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
	VULKAN_PRESENT_MODE_TYPE presentMode;

	//The surface capabilities.
	VULKAN_SURFACE_CAPABILITIES_TYPE surfaceCapabilities;

	//The surface format.
	VULKAN_SURFACE_FORMAT_TYPE surfaceFormat;

	//The index for the graphics queue family.
	uint32 graphicsQueueFamilyIndex{ UINT32_MAXIMUM };

	//The index for the present queue family.
	uint32 presentQueueFamilyIndex{ UINT32_MAXIMUM };

	//The index for the transfer queue family.
	uint32 transferQueueFamilyIndex{ UINT32_MAXIMUM };

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
	VULKAN_SURFACE_FORMAT_TYPE GetMostOptimalSurfaceFormat() const NOEXCEPT;

	/*
	*	Given a physical device and a surface, returns the most optimal present mode.
	*/
	VULKAN_PRESENT_MODE_TYPE GetMostOptimalPresentMode() const NOEXCEPT;

};