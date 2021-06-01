#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanSwapChain.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanSemaphore.h>
#include <Rendering/Abstraction/Vulkan/VulkanSurface.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

/*
*	Initializes this Vulkan swap chain.
*/
void VulkanSwapchain::Initialize() NOEXCEPT
{
	//Find the most optimal swap extent.
	FindMostOptimalSwapExtent();

	//Set up the queue family indices.
	const uint32 graphicsQueueFamilyIndex = VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::GRAPHICS);
	const uint32 presentQueueFamilyIndex = VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::PRESENT);

	uint32 queueFamilyIndices[] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

	//Create the swap chain create info.
	VkSwapchainCreateInfoKHR swapChainCreateInfo;
	CreateSwapChainCreateInfo(swapChainCreateInfo, queueFamilyIndices, graphicsQueueFamilyIndex, presentQueueFamilyIndex);

	//Create the swap chain!
	VULKAN_ERROR_CHECK(vkCreateSwapchainKHR(VulkanInterface::Instance->GetLogicalDevice().Get(), &swapChainCreateInfo, nullptr, &_VulkanSwapChain));

	//Query the swap chain images.
	VULKAN_ERROR_CHECK(vkGetSwapchainImagesKHR(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanSwapChain, &_NumberOfSwapChainImages, nullptr));

	_SwapChainImages.Upsize<false>(_NumberOfSwapChainImages);
	VULKAN_ERROR_CHECK(vkGetSwapchainImagesKHR(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanSwapChain, &_NumberOfSwapChainImages, _SwapChainImages.Data()));

	//Create the image views.
	_SwapChainImageViews.Upsize<false>(_NumberOfSwapChainImages);

	for (uint32 i = 0; i < _NumberOfSwapChainImages; ++i)
	{
		VulkanUtilities::CreateVulkanImageView(_SwapChainImages[i], VK_IMAGE_VIEW_TYPE_2D, VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, _SwapChainImageViews[i]);
	}
}

/*
*	Releases this Vulkan swap chain.
*/
void VulkanSwapchain::Release() NOEXCEPT
{
	//Destroy all image views.
	for (auto &swapChainImageView : _SwapChainImageViews)
	{
		vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), swapChainImageView, nullptr);
	}

	//Destroy the Vulkan swap chain.
	vkDestroySwapchainKHR(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanSwapChain, nullptr);
}

/*
*	Updates the next image index in the swap chain.
*/
void VulkanSwapchain::UpdateNextImageIndex(const VulkanSemaphore *const RESTRICT imageAvailableSemaphore) NOEXCEPT
{
	VULKAN_ERROR_CHECK(vkAcquireNextImageKHR(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanSwapChain, UINT64_MAXIMUM, imageAvailableSemaphore->Get(), VK_NULL_HANDLE, &_CurrentImageIndex));
}

/*
*	Presents the current image to the screen.
*/
void VulkanSwapchain::Present(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) NOEXCEPT
{
	//Present on the present queue!
	VulkanInterface::Instance->GetPresentQueue()->Present(renderFinishedSemaphore, &_VulkanSwapChain, &_CurrentImageIndex);
}

/*
*	Finds the most optimal swap extent.
*/
void VulkanSwapchain::FindMostOptimalSwapExtent() NOEXCEPT
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VULKAN_ERROR_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanInterface::Instance->GetPhysicalDevice().Get(), VulkanInterface::Instance->GetSurface().Get(), &surfaceCapabilities));

	_SwapExtent.width = CatalystBaseMath::Clamp<uint32>(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
	_SwapExtent.height = CatalystBaseMath::Clamp<uint32>(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
}

/*
*	Creates the swap chain create info.
*/
void VulkanSwapchain::CreateSwapChainCreateInfo(VkSwapchainCreateInfoKHR &swapChainCreateInfo, uint32 *RESTRICT queueFamilyIndices, const uint32 graphicsQueueFamilyIndex, const uint32 presentQueueFamilyIndex) const NOEXCEPT
{
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.pNext = nullptr;
	swapChainCreateInfo.flags = 0;
	swapChainCreateInfo.surface = VulkanInterface::Instance->GetSurface().Get();

	const VkSurfaceCapabilitiesKHR &surface_capabilities{ VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceCapabilities() };
	const VkSurfaceFormatKHR &surface_format{ VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat() };
	const VkPresentModeKHR &present_mode{ VulkanInterface::Instance->GetPhysicalDevice().GetPresentMode() };

	uint32 minimumImageCount = CatalystBaseMath::Maximum<uint32>(2, surface_capabilities.minImageCount);

	if (surface_capabilities.maxImageCount > 0 && minimumImageCount > surface_capabilities.maxImageCount)
		minimumImageCount = surface_capabilities.maxImageCount;

	swapChainCreateInfo.minImageCount = minimumImageCount;
	swapChainCreateInfo.imageFormat = surface_format.format;
	swapChainCreateInfo.imageColorSpace = surface_format.colorSpace;
	swapChainCreateInfo.imageExtent = _SwapExtent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
	{
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	else
	{
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 1;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	swapChainCreateInfo.preTransform = surface_capabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode = present_mode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
}
#endif