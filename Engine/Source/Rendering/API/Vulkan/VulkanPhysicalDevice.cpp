//Header file.
#include <Rendering/API/Vulkan/VulkanPhysicalDevice.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan physical device.
*/
void VulkanPhysicalDevice::Initialize() NOEXCEPT
{
	//Get the number of available physical devices.
	uint32 availablePhysicalDeviceCount{ 0 };
	VULKAN_ERROR_CHECK(vkEnumeratePhysicalDevices(VulkanInterface::Instance->GetInstance().Get(), &availablePhysicalDeviceCount, nullptr));

	//Get all available physical devices.
	DynamicArray<VkPhysicalDevice> availablePhysicalDevices;
	availablePhysicalDevices.UpsizeFast(availablePhysicalDeviceCount);
	VULKAN_ERROR_CHECK(vkEnumeratePhysicalDevices(VulkanInterface::Instance->GetInstance().Get(), &availablePhysicalDeviceCount, availablePhysicalDevices.Data()));

	//Create and fill a list of suitable physical devices.
	DynamicArray<VkPhysicalDevice> suitablePhysicalDevices;
	suitablePhysicalDevices.Reserve(availablePhysicalDeviceCount);

	for (const auto &availablePhysicalDevice : availablePhysicalDevices)
	{
		//Check if this physical device is suitable.
		if (IsPhysicalDeviceSuitable(availablePhysicalDevice))
		{
			suitablePhysicalDevices.EmplaceFast(availablePhysicalDevice);
		}
	}

	//Of all suitable physical devices, choose the most suitable one.
	vulkanPhysicalDevice = GetMostSuitableDevice(suitablePhysicalDevices);

	//Now that a suitable physical device is choosen, get the physical device features.
	vkGetPhysicalDeviceFeatures(vulkanPhysicalDevice, &physicalDeviceFeatures);

	//Get the physical device memory properties.
	vkGetPhysicalDeviceMemoryProperties(vulkanPhysicalDevice, &physicalDeviceMemoryProperties);

	//Get the physical device properties.
	vkGetPhysicalDeviceProperties(vulkanPhysicalDevice, &physicalDeviceProperties);

	//Get the most optimal present mode.
	presentMode = GetMostOptimalPresentMode();

	//Get the surface capabilities.
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &surfaceCapabilities));

	//Get the most optimal surface format.
	surfaceFormat = GetMostOptimalSurfaceFormat();
}

/*
*	Given a Vulkan physical device, return if it is suitable for this game.
*/
bool VulkanPhysicalDevice::IsPhysicalDeviceSuitable(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT
{
	//Check if it has the required features.
	if (!HasRequiredFeatures(vulkanPhysicalDevice))
	{
		return false;
	}

	//Check if the physical device has proper swap chain support.
	if (!HasProperSwapChainSupport(vulkanPhysicalDevice))
	{
		return false;
	}

	//If the device has not failed on any of the previous checks, it is deemed suitable.
	return true;
}

/*
*	Given a Vulkan physical device, return if it has the required features.
*/
bool VulkanPhysicalDevice::HasRequiredFeatures(const VkPhysicalDevice vulkanPhysicalDevice) const NOEXCEPT
{
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(vulkanPhysicalDevice, &features);

	return	features.geometryShader == VK_TRUE
			&& features.tessellationShader == VK_TRUE;
}

/*
*	Given a Vulkan physical device and a Vulkan surface, return if the Physical device has the proper swap chain support.
*/
bool VulkanPhysicalDevice::HasProperSwapChainSupport(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT
{
	//Query for format support.
	uint32 formatsCount = 0; 
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &formatsCount, nullptr));

	DynamicArray<VkSurfaceFormatKHR> formats;
	formats.UpsizeFast(formatsCount);

	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &formatsCount, formats.Data()));

	//Query for present mode support.
	uint32 presentModesCount = 0;
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &presentModesCount, nullptr));

	DynamicArray<VkPresentModeKHR> presentModes;
	presentModes.UpsizeFast(presentModesCount);

	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &presentModesCount, presentModes.Data()));

	//If there are at least one format and one present mode available, this physical device is considered to have proper swap chain support.
	return !formats.Empty() && !presentModes.Empty();
}

/*
*	Given a list of suitable physical devices, return most suitable physical device.
*/
VkPhysicalDevice VulkanPhysicalDevice::GetMostSuitableDevice(const DynamicArray<VkPhysicalDevice> &suitablePhysicalDevices) const NOEXCEPT
{
	//For now just return the first one.
	return suitablePhysicalDevices[0];
}

/*
*	Given a physical device and a surface, returns the most optimal surface format.
*/
VkSurfaceFormatKHR VulkanPhysicalDevice::GetMostOptimalSurfaceFormat() const NOEXCEPT
{
	//Query for format support.
	uint32 availableFormatsCount = 0;
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availableFormatsCount, nullptr));

	DynamicArray<VkSurfaceFormatKHR> availableFormats;
	availableFormats.UpsizeFast(availableFormatsCount);

	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availableFormatsCount, availableFormats.Data()));

	//Find the most optimal surface format.
	if (availableFormats.Size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

/*
*	Given a physical device and a surface, returns the most optimal present mode.
*/
VkPresentModeKHR VulkanPhysicalDevice::GetMostOptimalPresentMode() const NOEXCEPT
{
	//Query for present mode support.
	uint32 availablePresentModesCount = 0;
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availablePresentModesCount, nullptr));

	DynamicArray<VkPresentModeKHR> availablePresentModes;
	availablePresentModes.UpsizeFast(availablePresentModesCount);

	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availablePresentModesCount, availablePresentModes.Data()));

	//Find the most optimal present mode.
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}