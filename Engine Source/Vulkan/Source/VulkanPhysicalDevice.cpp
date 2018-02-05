//Header file.
#include <VulkanPhysicalDevice.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanPhysicalDevice::VulkanPhysicalDevice() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanPhysicalDevice::~VulkanPhysicalDevice() NOEXCEPT
{

}

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
	availablePhysicalDevices.Resize(availablePhysicalDeviceCount);
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
	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_CAPABILITIES(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &surfaceCapabilities));

	//Get the most optimal surface format.
	surfaceFormat = GetMostOptimalSurfaceFormat();

	//Retrieve the queue family properties.
	uint32 queueFamilyCount{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(vulkanPhysicalDevice, &queueFamilyCount, nullptr);

	DynamicArray<VkQueueFamilyProperties> queueFamilyProperties;
	queueFamilyProperties.Resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vulkanPhysicalDevice, &queueFamilyCount, queueFamilyProperties.Data());

	//Find the queue family for the graphics.
	uint32 queueFamilyCounter{ 0 };

	for (auto &queueFamilyProperty : queueFamilyProperties)
	{
		if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsQueueFamilyIndex = queueFamilyCounter;
		}

		if (queueFamilyProperty.queueCount > 0 && queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			transferQueueFamilyIndex = queueFamilyCounter;
		}

		VkBool32 presentSupport = false;

		VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_SUPPORT(vulkanPhysicalDevice, queueFamilyCounter, VulkanInterface::Instance->GetSurface().Get(), &presentSupport));

		if (queueFamilyProperty.queueCount > 0 && presentSupport)
		{
			presentQueueFamilyIndex = queueFamilyCounter;
		}

		++queueFamilyCounter;
	}
}

/*
*	Given a Vulkan physical device, return if it is suitable for this game.
*/
bool VulkanPhysicalDevice::IsPhysicalDeviceSuitable(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT
{
	//Check if it has the required extensions.
	if (!HasRequiredExtensions(vulkanPhysicalDevice))
		return false;

	//Check if the physical device has proper swap chain support.
	if (!HasProperSwapChainSupport(vulkanPhysicalDevice))
		return false;

	//If the device has not failed on any of the previous checks, it is deemed suitable.
	return true;
}

/*
*	Given a Vulkan physical device, return if it has the required extensions.
*/
bool VulkanPhysicalDevice::HasRequiredExtensions(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT
{
	//Define the list of the required extensions.
	std::set<DynamicString> requiredExtensions{ VULKAN_SWAPCHAIN_EXTENSION_NAME };

	//Get the extension count.
	uint32_t extensionCount;
	VULKAN_ERROR_CHECK(vkEnumerateDeviceExtensionProperties(vulkanPhysicalDevice, nullptr, &extensionCount, nullptr));

	//Get the available extensions.
	DynamicArray<VkExtensionProperties> availableExtensions;
	availableExtensions.Resize(extensionCount);
	VULKAN_ERROR_CHECK(vkEnumerateDeviceExtensionProperties(vulkanPhysicalDevice, nullptr, &extensionCount, availableExtensions.Data()));

	//Iterate over all available extensions and match them with the required extensions.
	for (auto &availableExtension : availableExtensions)
	{
		requiredExtensions.erase(availableExtension.extensionName);
	}

	//If the required extensions list has been cleared, that means that all required extensions are available.
	return requiredExtensions.empty();
}

/*
*	Given a Vulkan physical device and a Vulkan surface, return if the Physical device has the proper swap chain support.
*/
bool VulkanPhysicalDevice::HasProperSwapChainSupport(const VkPhysicalDevice &vulkanPhysicalDevice) const NOEXCEPT
{
	//Query for format support.
	uint32 formatsCount = 0; 
	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_FORMATS(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &formatsCount, nullptr));

	DynamicArray<VULKAN_SURFACE_FORMAT_TYPE> formats;
	formats.Resize(formatsCount);

	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_FORMATS(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &formatsCount, formats.Data()));

	//Query for present mode support.
	uint32 presentModesCount = 0;
	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &presentModesCount, nullptr));

	DynamicArray<VULKAN_PRESENT_MODE_TYPE> presentModes;
	presentModes.Resize(presentModesCount);

	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &presentModesCount, presentModes.Data()));

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
VULKAN_SURFACE_FORMAT_TYPE VulkanPhysicalDevice::GetMostOptimalSurfaceFormat() const NOEXCEPT
{
	//Query for format support.
	uint32 availableFormatsCount = 0;
	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_FORMATS(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availableFormatsCount, nullptr));

	DynamicArray<VULKAN_SURFACE_FORMAT_TYPE> availableFormats;
	availableFormats.Resize(availableFormatsCount);

	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_FORMATS(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availableFormatsCount, availableFormats.Data()));

	//Find the most optimal surface format.
	if (availableFormats.Size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VULKAN_COLOR_SPACE_SRGB_NONLINEAR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VULKAN_COLOR_SPACE_SRGB_NONLINEAR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

/*
*	Given a physical device and a surface, returns the most optimal present mode.
*/
VULKAN_PRESENT_MODE_TYPE VulkanPhysicalDevice::GetMostOptimalPresentMode() const NOEXCEPT
{
	//Query for present mode support.
	uint32 availablePresentModesCount = 0;
	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availablePresentModesCount, nullptr));

	DynamicArray<VULKAN_PRESENT_MODE_TYPE> availablePresentModes;
	availablePresentModes.Resize(availablePresentModesCount);

	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availablePresentModesCount, availablePresentModes.Data()));

	//Find the most optimal present mode.
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VULKAN_PRESENT_MODE_MAILBOX)
		{
			return availablePresentMode;
		}
	}

	return VULKAN_PRESENT_MODE_FIFO;
}