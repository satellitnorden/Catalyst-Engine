#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanPhysicalDevice final
{

public:

	/*
	*	Returns the underlying Vulkan physical device object.
	*/
	const VkPhysicalDevice& Get() const NOEXCEPT { return _VulkanPhysicalDevice; }

	/*
	*	Initializes this Vulkan physical device.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Returns the physical device features.
	*/
	const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const NOEXCEPT { return _PhysicalDeviceFeatures; }

	/*
	*	Returns the physical device memory properties.
	*/
	const VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() const NOEXCEPT { return _PhysicalDeviceMemoryProperties; }

	/*
	*	Returns the physical device properties.
	*/
	const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const NOEXCEPT { return _PhysicalDeviceProperties; }

	/*
	*	Returns the ray tracing properties.
	*/
	const VkPhysicalDeviceRayTracingPropertiesNV& GetRayTracingProperties() const NOEXCEPT { return _RayTracingProperties; }

	/*
	*	Returns the surface format.
	*/
	const VkPresentModeKHR& GetPresentMode() const NOEXCEPT { return _PresentMode; }

	/*
	*	Returns the surface capabilities.
	*/
	const VkSurfaceCapabilitiesKHR& GetSurfaceCapabilities() const NOEXCEPT { return _SurfaceCapabilities; }

	/*
	*	Returns the surface format.
	*/
	const VkSurfaceFormatKHR& GetSurfaceFormat() const NOEXCEPT { return _SurfaceFormat; }

	/*
	*	Returns whether or not this Vulkan physical device has ray tracing support.
	*/
	FORCE_INLINE NO_DISCARD bool HasRayTracingSupport() const NOEXCEPT
	{
		return _HasRayTracingSupport;
	}

private:

	//The underlying Vulkan physical device.
	VkPhysicalDevice _VulkanPhysicalDevice;

	//The physical device features.
	VkPhysicalDeviceFeatures _PhysicalDeviceFeatures;

	//The physical device memory properties.
	VkPhysicalDeviceMemoryProperties _PhysicalDeviceMemoryProperties;

	//The physical device properties.
	VkPhysicalDeviceProperties _PhysicalDeviceProperties;

	//The ray tracing properties.
	VkPhysicalDeviceRayTracingPropertiesNV _RayTracingProperties;

	//The present mode.
	VkPresentModeKHR _PresentMode;

	//The surface capabilities.
	VkSurfaceCapabilitiesKHR _SurfaceCapabilities;

	//The surface format.
	VkSurfaceFormatKHR _SurfaceFormat;

	//Denotes if this Vulkan physical device has ray tracing support.
	bool _HasRayTracingSupport{ false };

	/*
	*	Given a Vulkan physical device, return if it is suitable for this game.
	*/
	bool IsPhysicalDeviceSuitable(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT;

	/*
	*	Given a Vulkan physical device, return if it has the required features.
	*/
	bool HasRequiredFeatures(const VkPhysicalDevice vulkanPhysicalDevice) const NOEXCEPT;

	/*
	*	Given a Vulkan physical device and a Vulkan surface, return if the Physical device has the proper swap chain support.
	*/
	bool HasProperSwapChainSupport(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT;

	/*
	*	Given a Vulkan physical device and an extension name, returns if the Vulkan physical device has that extension.
	*/
	bool HasExtension(const VkPhysicalDevice &device, const char *const RESTRICT extension) const NOEXCEPT;

	/*
	*	Given a list of suitable physical devices, return most suitable physical device.
	*/
	VkPhysicalDevice GetMostSuitableDevice(const DynamicArray<VkPhysicalDevice> &suitable_physical_devices) const NOEXCEPT;

	/*
	*	Given a physical device and a surface, returns the most optimal surface format.
	*/
	VkSurfaceFormatKHR GetMostOptimalSurfaceFormat() const NOEXCEPT;

	/*
	*	Given a physical device and a surface, returns the most optimal present mode.
	*/
	VkPresentModeKHR GetMostOptimalPresentMode() const NOEXCEPT;

};
#endif