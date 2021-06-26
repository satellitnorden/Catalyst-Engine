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
	*	Returns whether or not this Vulkan physical device has multiview support.
	*/
	FORCE_INLINE NO_DISCARD bool HasMultiviewSupport() const NOEXCEPT
	{
		return _HasMultiviewSupport;
	}

	/*
	*	Returns the multiview features.
	*/
	FORCE_INLINE NO_DISCARD const VkPhysicalDeviceMultiviewFeatures &GetMultiviewFeatures() const NOEXCEPT
	{
		return _MultiviewFeatures;
	}

	/*
	*	Returns the multiview properties.
	*/
	FORCE_INLINE NO_DISCARD const VkPhysicalDeviceMultiviewProperties &GetMultiviewProperties() const NOEXCEPT
	{
		return _MultiviewProperties;
	}

	/*
	*	Returns whether or not this Vulkan physical device has ray tracing support.
	*/
	FORCE_INLINE NO_DISCARD bool HasRayTracingSupport() const NOEXCEPT
	{
		return _HasRayTracingSupport;
	}

	/*
	*	Returns the ray tracing properties.
	*/
	FORCE_INLINE NO_DISCARD const VkPhysicalDeviceRayTracingPropertiesNV &GetRayTracingProperties() const NOEXCEPT
	{
		return _RayTracingProperties;
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

	//The present mode.
	VkPresentModeKHR _PresentMode;

	//The surface capabilities.
	VkSurfaceCapabilitiesKHR _SurfaceCapabilities;

	//The surface format.
	VkSurfaceFormatKHR _SurfaceFormat;

	//Denotes whether or not this Vulkan physical device has multiview support.
	bool _HasMultiviewSupport{ false };

	//The multiview features.
	VkPhysicalDeviceMultiviewFeatures _MultiviewFeatures;

	//The multiview properties.
	VkPhysicalDeviceMultiviewProperties _MultiviewProperties;

	//Denotes whether or not this Vulkan physical device has ray tracing support.
	bool _HasRayTracingSupport{ false };

	//The ray tracing properties.
	VkPhysicalDeviceRayTracingPropertiesNV _RayTracingProperties;

	/*
	*	Given a Vulkan physical device, return if it is suitable for this game.
	*/
	bool IsPhysicalDeviceSuitable(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT;

	/*
	*	Given a Vulkan physical device, return if it has the required features.
	*/
	bool HasRequiredFeatures(const VkPhysicalDevice vulkanPhysicalDevice) const NOEXCEPT;

#if !VULKAN_RECEIVES_SWAPCHAIN_FROM_PLATFORM
	/*
	*	Given a Vulkan physical device and a Vulkan surface, return if the Physical device has the proper swap chain support.
	*/
	bool HasProperSwapChainSupport(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT;
#endif

	/*
	*	Given a Vulkan physical device and an extension name, returns if the Vulkan physical device has that extension.
	*/
	bool HasExtension(const VkPhysicalDevice &device, const char *const RESTRICT extension) const NOEXCEPT;

	/*
	*	Given a list of suitable physical devices, return most suitable physical device.
	*/
	VkPhysicalDevice GetMostSuitableDevice(const DynamicArray<VkPhysicalDevice> &suitable_physical_devices) const NOEXCEPT;

#if !VULKAN_RECEIVES_SWAPCHAIN_FROM_PLATFORM
	/*
	*	Given a physical device and a surface, returns the most optimal surface format.
	*/
	VkSurfaceFormatKHR GetMostOptimalSurfaceFormat() const NOEXCEPT;
#endif

#if !VULKAN_RECEIVES_SWAPCHAIN_FROM_PLATFORM
	/*
	*	Given a physical device and a surface, returns the most optimal present mode.
	*/
	VkPresentModeKHR GetMostOptimalPresentMode() const NOEXCEPT;
#endif

};
#endif