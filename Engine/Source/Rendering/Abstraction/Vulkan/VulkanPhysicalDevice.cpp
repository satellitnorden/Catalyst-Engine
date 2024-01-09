#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>

//Core.
#include <Core/Utilities/StringUtilities.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

//Vulkan physical device logic.
namespace VulkanPhysicalDeviceLogic
{

	/*
	*	Returns a physical device's multiview features.
	*/
	FORCE_INLINE void GetMultiviewFeatures(const VkPhysicalDevice physical_device, VkPhysicalDeviceMultiviewFeatures *const RESTRICT multiview_features) NOEXCEPT
	{
		multiview_features->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
		multiview_features->pNext = nullptr;

		VkPhysicalDeviceFeatures2 properties{ };
		properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		properties.pNext = multiview_features;

		vkGetPhysicalDeviceFeatures2(physical_device, &properties);
	}

	/*
	*	Returns a physical device's multiview properties.
	*/
	FORCE_INLINE void GetMultiviewProperties(const VkPhysicalDevice physical_device, VkPhysicalDeviceMultiviewProperties *const RESTRICT multiview_properties) NOEXCEPT
	{
		multiview_properties->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES;
		multiview_properties->pNext = nullptr;

		VkPhysicalDeviceProperties2 properties;
		properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		properties.pNext = multiview_properties;

		vkGetPhysicalDeviceProperties2(physical_device, &properties);
	}

	/*
	*	Returns a physical device's ray tracing properties.
	*/
	FORCE_INLINE void GetRayTracingProperties(const VkPhysicalDevice physical_device, VkPhysicalDeviceRayTracingPropertiesNV *const RESTRICT ray_tracing_properties) NOEXCEPT
	{
		ray_tracing_properties->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;
		ray_tracing_properties->pNext = nullptr;

		VkPhysicalDeviceProperties2 properties;
		properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		properties.pNext = ray_tracing_properties;

		vkGetPhysicalDeviceProperties2(physical_device, &properties);
	}

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
	availablePhysicalDevices.Upsize<false>(availablePhysicalDeviceCount);
	VULKAN_ERROR_CHECK(vkEnumeratePhysicalDevices(VulkanInterface::Instance->GetInstance().Get(), &availablePhysicalDeviceCount, availablePhysicalDevices.Data()));

	//Create and fill a list of suitable physical devices.
	DynamicArray<VkPhysicalDevice> suitablePhysicalDevices;
	suitablePhysicalDevices.Reserve(availablePhysicalDeviceCount);

	for (const auto &availablePhysicalDevice : availablePhysicalDevices)
	{
		//Check if this physical device is suitable.
		if (IsPhysicalDeviceSuitable(availablePhysicalDevice))
		{
			suitablePhysicalDevices.Emplace(availablePhysicalDevice);
		}
	}

	//Of all suitable physical devices, choose the most suitable one.
	_VulkanPhysicalDevice = GetMostSuitableDevice(suitablePhysicalDevices);

	//Now that a suitable physical device is choosen, get the physical device features.
	vkGetPhysicalDeviceFeatures(_VulkanPhysicalDevice, &_PhysicalDeviceFeatures);

	//Get the physical device memory properties.
	vkGetPhysicalDeviceMemoryProperties(_VulkanPhysicalDevice, &_PhysicalDeviceMemoryProperties);

	//Get the physical device properties.
	vkGetPhysicalDeviceProperties(_VulkanPhysicalDevice, &_PhysicalDeviceProperties);

	//Does the chosen Vulkan physical device have multiview support?
	_HasMultiviewSupport = HasExtension(_VulkanPhysicalDevice, VK_KHR_MULTIVIEW_EXTENSION_NAME);

	//Retrieve the multiview features/properties.
	if (_HasMultiviewSupport)
	{
		VulkanPhysicalDeviceLogic::GetMultiviewFeatures(_VulkanPhysicalDevice, &_MultiviewFeatures);
		VulkanPhysicalDeviceLogic::GetMultiviewProperties(_VulkanPhysicalDevice, &_MultiviewProperties);
	}

	else
	{
		Memory::Set(&_MultiviewFeatures, 0, sizeof(VkPhysicalDeviceMultiviewFeatures));
		Memory::Set(&_MultiviewProperties, 0, sizeof(VkPhysicalDeviceMultiviewProperties));
	}

	//Does the chosen Vulkan physical device have ray tracing support?
	_HasRayTracingSupport = HasExtension(_VulkanPhysicalDevice, VK_NV_RAY_TRACING_EXTENSION_NAME);

	//Get the ray tracing properties.
	if (_HasRayTracingSupport)
	{
		VulkanPhysicalDeviceLogic::GetRayTracingProperties(_VulkanPhysicalDevice, &_RayTracingProperties);
	}

	else
	{
		Memory::Set(&_RayTracingProperties, 0, sizeof(VkPhysicalDeviceRayTracingPropertiesNV));
	}

	//Get the most optimal present mode.
	_PresentMode = GetMostOptimalPresentMode();

	//Get the surface capabilities.
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_VulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &_SurfaceCapabilities));

	//Get the most optimal surface format.
	_SurfaceFormat = GetMostOptimalSurfaceFormat();
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
	//Yes. (:
	return true;
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
	formats.Upsize<false>(formatsCount);

	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &formatsCount, formats.Data()));

	//Query for present mode support.
	uint32 presentModesCount = 0;
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &presentModesCount, nullptr));

	DynamicArray<VkPresentModeKHR> presentModes;
	presentModes.Upsize<false>(presentModesCount);

	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &presentModesCount, presentModes.Data()));

	//If there are at least one format and one present mode available, this physical device is considered to have proper swap chain support.
	return !formats.Empty() && !presentModes.Empty();
}

/*
*	Given a Vulkan physical device and an extension name, returns if the Vulkan physical device has that extension.
*/
bool VulkanPhysicalDevice::HasExtension(const VkPhysicalDevice &device, const char *const RESTRICT extension) const NOEXCEPT
{
	//Retrieve the available extensions.
	uint32 number_of_available_extensions;
	VULKAN_ERROR_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &number_of_available_extensions, nullptr));

	DynamicArray<VkExtensionProperties> available_extensions;
	available_extensions.Upsize<false>(number_of_available_extensions);
	VULKAN_ERROR_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &number_of_available_extensions, available_extensions.Data()));

	//Go through all the available extensions and compare them.
	for (const VkExtensionProperties &available_extension : available_extensions)
	{
		if (StringUtilities::IsEqual(available_extension.extensionName, extension))
		{
			return true;
		}
	}

	//The extension was not found. ):
	return false;
}

/*
*	Given a list of suitable physical devices, return most suitable physical device.
*/
VkPhysicalDevice VulkanPhysicalDevice::GetMostSuitableDevice(const DynamicArray<VkPhysicalDevice> &suitable_physical_devices) const NOEXCEPT
{
	//If there's only one suitable physical device, then go with that one.
	if (suitable_physical_devices.Size() == 1)
	{
		return suitable_physical_devices[0];
	}

	//Calculate the score for all suitable physical devices.
	DynamicArray<uint64> suitable_physical_device_scores;
	suitable_physical_device_scores.Upsize<false>(suitable_physical_devices.Size());

	for (uint64 i{ 0 }, size{ suitable_physical_devices.Size() }; i < size; ++i)
	{
		//Get the physical device features.
		VkPhysicalDeviceFeatures physical_device_features;
		vkGetPhysicalDeviceFeatures(suitable_physical_devices[i], &physical_device_features);

		//Get the physical device memory properties.
		VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(suitable_physical_devices[i], &physical_device_memory_properties);

		//Get the physical device properties.
		VkPhysicalDeviceProperties physical_device_properties;
		vkGetPhysicalDeviceProperties(suitable_physical_devices[i], &physical_device_properties);

		//Start off at zero.
		suitable_physical_device_scores[i] = 0;

		//Discrete GPU's are preferred.
		suitable_physical_device_scores[i] += physical_device_properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1 : 0;
	}

	//Find the highest scoring physical device.
	uint64 highest_score{ 0 };
	const VkPhysicalDevice *RESTRICT most_suitable_physical_device{ nullptr };

	for (uint64 i{ 0 }, size{ suitable_physical_devices.Size() }; i < size; ++i)
	{
		if (highest_score < suitable_physical_device_scores[i])
		{
			highest_score = suitable_physical_device_scores[i];
			most_suitable_physical_device = &suitable_physical_devices[i];
		}
	}

	//For now just return the first one.
	return *most_suitable_physical_device;
}

/*
*	Given a physical device and a surface, returns the most optimal surface format.
*/
VkSurfaceFormatKHR VulkanPhysicalDevice::GetMostOptimalSurfaceFormat() const NOEXCEPT
{
	//Query for format support.
	uint32 availableFormatsCount = 0;
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_VulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availableFormatsCount, nullptr));

	DynamicArray<VkSurfaceFormatKHR> availableFormats;
	availableFormats.Upsize<false>(availableFormatsCount);

	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_VulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &availableFormatsCount, availableFormats.Data()));

	//Find the most optimal surface format.
	if (availableFormats.Size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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
	uint32 number_of_available_present_modes{ 0 };
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_VulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &number_of_available_present_modes, nullptr));

	DynamicArray<VkPresentModeKHR> available_present_modes;
	available_present_modes.Upsize<false>(number_of_available_present_modes);

	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_VulkanPhysicalDevice, VulkanInterface::Instance->GetSurface().Get(), &number_of_available_present_modes, available_present_modes.Data()));

	//Find the most optimal present mode.
	/*
	for (const VkPresentModeKHR available_present_mode : available_present_modes)
	{
		if (available_present_mode == VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		{
			return available_present_mode;
		}
	}
	*/

	return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
}
#endif