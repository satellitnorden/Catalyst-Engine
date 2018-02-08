//Header file.
#include <VulkanSwapChain.h>

//Vulkan.
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>
#include <VulkanSemaphore.h>
#include <VulkanSurface.h>
#include <VulkanUtilities.h>

/*
*	Default constructor.
*/
VulkanSwapchain::VulkanSwapchain() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanSwapchain::~VulkanSwapchain() NOEXCEPT
{

}

/*
*	Initializes this Vulkan swap chain.
*/
void VulkanSwapchain::Initialize() NOEXCEPT
{
	//Find the most optimal swap extent.
	FindMostOptimalSwapExtent();

	//Set up the queue family indices.
	const uint32 graphicsQueueFamilyIndex = VulkanInterface::Instance->GetPhysicalDevice().GetGraphicsQueueFamilyIndex();
	const uint32 presentQueueFamilyIndex = VulkanInterface::Instance->GetPhysicalDevice().GetPresentQueueFamilyIndex();

	uint32 queueFamilyIndices[] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

	//Create the swap chain create info.
	VULKAN_SWAPCHAIN_CREATE_INFO_TYPE swapChainCreateInfo;
	CreateSwapChainCreateInfo(swapChainCreateInfo, queueFamilyIndices, graphicsQueueFamilyIndex, presentQueueFamilyIndex);

	//Create the swap chain!
	VULKAN_ERROR_CHECK(VULKAN_CREATE_SWAPCHAIN(VulkanInterface::Instance->GetLogicalDevice().Get(), &swapChainCreateInfo, nullptr, &vulkanSwapChain));

	//Query the swap chain images.
	uint32 swapChainImagesCount = 0;
	
	VULKAN_ERROR_CHECK(VULKAN_GET_SWAPCHAIN_IMAGES(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanSwapChain, &swapChainImagesCount, nullptr));

	swapChainImages.Resize(swapChainImagesCount);
	VULKAN_ERROR_CHECK(VULKAN_GET_SWAPCHAIN_IMAGES(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanSwapChain, &swapChainImagesCount, swapChainImages.Data()));

	//Create the image views.
	swapChainImageViews.Resize(swapChainImagesCount);

	for (uint32 i = 0; i < swapChainImagesCount; ++i)
	{
		VulkanUtilities::CreateVulkanImageView(swapChainImages[i], VK_IMAGE_VIEW_TYPE_2D, VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format, VK_IMAGE_ASPECT_COLOR_BIT, 1, swapChainImageViews[i]);
	}

	//Initialize the depth buffer.
	vulkanDepthBuffer.Initialize(swapExtent);
}

/*
*	Releases this Vulkan swap chain.
*/
void VulkanSwapchain::Release() NOEXCEPT
{
	//Release the depth buffer.
	vulkanDepthBuffer.Release();

	//Destroy all image views.
	for (auto &swapChainImageView : swapChainImageViews)
	{
		vkDestroyImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), swapChainImageView, nullptr);
	}

	//Destroy the Vulkan swap chain.
	VULKAN_DESTROY_SWAPCHAIN(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanSwapChain, nullptr);
}

/*
*	Updates the next image index in the swap chain.
*/
void VulkanSwapchain::UpdateNextImageIndex(const VulkanSemaphore *const RESTRICT imageAvailableSemaphore) NOEXCEPT
{
	VULKAN_ERROR_CHECK(VULKAN_AQUIRE_NEXT_IMAGE(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanSwapChain, UINT64_MAXIMUM, imageAvailableSemaphore->Get(), VK_NULL_HANDLE, &currentImageIndex));
}

/*
*	Presents the current image to the screen.
*/
void VulkanSwapchain::Present(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) NOEXCEPT
{
	//Create the present info.
	VULKAN_PRESENT_INFO_TYPE presentInfo;
	CreatePresentInfo(presentInfo, renderFinishedSemaphore);

	//Present on the present queue!
	VULKAN_ERROR_CHECK(VULKAN_QUEUUE_PRESENT(VulkanInterface::Instance->GetPresentQueue().Get(), &presentInfo));
}

/*
*	Finds the most optimal swap extent.
*/
void VulkanSwapchain::FindMostOptimalSwapExtent() NOEXCEPT
{
	VULKAN_SURFACE_CAPABILITIES_TYPE surfaceCapabilities;
	VULKAN_ERROR_CHECK(VULKAN_GET_PHYSICAL_DEVICE_SURFACE_CAPABILITIES(VulkanInterface::Instance->GetPhysicalDevice().Get(), VulkanInterface::Instance->GetSurface().Get(), &surfaceCapabilities));

	if (surfaceCapabilities.currentExtent.width != UINT32_MAXIMUM)
	{
		swapExtent = surfaceCapabilities.currentExtent;
	}

	else
	{
		VkExtent2D actualExtent;

		actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));

		swapExtent = actualExtent;
	}
}

/*
*	Creates the swap chain create info.
*/
void VulkanSwapchain::CreateSwapChainCreateInfo(VULKAN_SWAPCHAIN_CREATE_INFO_TYPE &swapChainCreateInfo, uint32 *RESTRICT queueFamilyIndices, const uint32 graphicsQueueFamilyIndex, const uint32 presentQueueFamilyIndex) const NOEXCEPT
{
	swapChainCreateInfo.sType = VULKAN_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO;
	swapChainCreateInfo.pNext = nullptr;
	swapChainCreateInfo.flags = 0;
	swapChainCreateInfo.surface = VulkanInterface::Instance->GetSurface().Get();

	const auto &surfaceCapabilities = VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceCapabilities();
	const auto &surfaceFormat = VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat();
	const auto &presentMode = VulkanInterface::Instance->GetPhysicalDevice().GetPresentMode();

	uint32 minimumImageCount = surfaceCapabilities.minImageCount + 1;

	if (surfaceCapabilities.maxImageCount > 0 && minimumImageCount > surfaceCapabilities.maxImageCount)
		minimumImageCount = surfaceCapabilities.maxImageCount;

	swapChainCreateInfo.minImageCount = minimumImageCount;
	swapChainCreateInfo.imageFormat = surfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent = swapExtent;
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
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = VULKAN_COMPOSITE_ALPHA_OPAQUE_BIT;
	swapChainCreateInfo.presentMode = presentMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
}

/*
*	Creates a present info.
*/
void VulkanSwapchain::CreatePresentInfo(VULKAN_PRESENT_INFO_TYPE &presentInfo, const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) const NOEXCEPT
{
	presentInfo.sType = VULKAN_STRUCTURE_TYPE_PRESENT_INFO;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = reinterpret_cast<const VkSemaphore *RESTRICT>(renderFinishedSemaphore);
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vulkanSwapChain;
	presentInfo.pImageIndices = &currentImageIndex;
	presentInfo.pResults = nullptr;
}