#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanInstance;
class VulkanSurface;

class VulkanPhysicalDevice
{

public:

	/*
	*	Default constructor.
	*/
	VulkanPhysicalDevice() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanPhysicalDevice() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan physical device object.
	*/
	const VkPhysicalDevice& Get() const CATALYST_NOEXCEPT { return vulkanPhysicalDevice; }

	/*
	*	Initializes this Vulkan physical device.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Returns the surface capabilities.
	*/
	const VULKAN_SURFACE_CAPABILITIES_TYPE& GetSurfaceCapabilities() const CATALYST_NOEXCEPT { return surfaceCapabilities; }

	/*
	*	Returns the surface format.
	*/
	const VULKAN_SURFACE_FORMAT_TYPE& GetSurfaceFormat() const CATALYST_NOEXCEPT { return surfaceFormat; }

	/*
	*	Returns the physical device properties.
	*/
	const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const CATALYST_NOEXCEPT { return physicalDeviceProperties; }

	/*
	*	Returns the surface format.
	*/
	const VULKAN_PRESENT_MODE_TYPE& GetPresentMode() const CATALYST_NOEXCEPT { return presentMode; }

	/*
	*	Returns the graphics queue family index.
	*/
	uint32 GetGraphicsQueueFamilyIndex() const CATALYST_NOEXCEPT { return graphicsQueueFamilyIndex; }

	/*
	*	Returns the present queue family index.
	*/
	uint32 GetPresentQueueFamilyIndex() const CATALYST_NOEXCEPT { return presentQueueFamilyIndex; }

	/*
	*	Returns the transfer queue family index.
	*/
	uint32 GetTransferQueueFamilyIndex() const CATALYST_NOEXCEPT { return transferQueueFamilyIndex; }

private:

	//The underlying Vulkan physical device.
	VkPhysicalDevice vulkanPhysicalDevice;

	//The surface capabilities.
	VULKAN_SURFACE_CAPABILITIES_TYPE surfaceCapabilities;

	//The surface format.
	VULKAN_SURFACE_FORMAT_TYPE surfaceFormat;

	//The physical device properties.
	VkPhysicalDeviceProperties physicalDeviceProperties;

	//The present mode.
	VULKAN_PRESENT_MODE_TYPE presentMode;

	//The index for the graphics queue family.
	uint32 graphicsQueueFamilyIndex;

	//The index for the present queue family.
	uint32 presentQueueFamilyIndex;

	//The index for the transfer queue family.
	uint32 transferQueueFamilyIndex;

	/*
	*	Given a Vulkan physical device, return if it is suitable for this game.
	*/
	bool IsPhysicalDeviceSuitable(const VkPhysicalDevice &vulkanPhysicalDevice) const CATALYST_NOEXCEPT;

	/*
	*	Given a Vulkan physical device, return if it has the required extensions.
	*/
	bool HasRequiredExtensions(const VkPhysicalDevice &vulkanPhysicalDevice) const CATALYST_NOEXCEPT;

	/*
	*	Given a Vulkan physical device and a Vulkan surface, return if the Physical device has the proper swap chain support.
	*/
	bool HasProperSwapChainSupport(const VkPhysicalDevice &vulkanPhysicalDevice) const CATALYST_NOEXCEPT;

	/*
	*	Given a list of suitable physical devices, return most suitable physical device.
	*/
	VkPhysicalDevice GetMostSuitableDevice(const DynamicArray<VkPhysicalDevice> &suitablePhysicalDevices) const CATALYST_NOEXCEPT;

	/*
	*	Given a physical device and a surface, returns the most optimal surface format.
	*/
	VULKAN_SURFACE_FORMAT_TYPE GetMostOptimalSurfaceFormat() const CATALYST_NOEXCEPT;

	/*
	*	Given a physical device and a surface, returns the most optimal present mode.
	*/
	VULKAN_PRESENT_MODE_TYPE GetMostOptimalPresentMode() const CATALYST_NOEXCEPT;

};