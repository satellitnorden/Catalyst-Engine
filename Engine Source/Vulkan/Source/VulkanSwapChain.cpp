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
VulkanSwapchain::VulkanSwapchain() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanSwapchain::~VulkanSwapchain() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan swap chain.
*/
void VulkanSwapchain::Initialize() CATALYST_NOEXCEPT
{
	//Find the most optimal swap extent.
	FindMostOptimalSwapExtent();

	//Set up the queue family indices.
	const uint32 graphicsQueueFamilyIndex = VulkanInterface::Instance->GetVulkanPhysicalDevice().GetGraphicsQueueFamilyIndex();
	const uint32 presentQueueFamilyIndex = VulkanInterface::Instance->GetVulkanPhysicalDevice().GetPresentQueueFamilyIndex();

	uint32 queueFamilyIndices[] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

	//Create the swap chain create info.
	VkSwapchainCreateInfoKHR swapChainCreateInfo;
	CreateSwapChainCreateInfo(swapChainCreateInfo, queueFamilyIndices, graphicsQueueFamilyIndex, presentQueueFamilyIndex);

	//Create the swap chain!
	const VkResult result = vkCreateSwapchainKHR(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &swapChainCreateInfo, nullptr, &vulkanSwapChain);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif

	//Query the swap chain images.
	uint32 swapChainImagesCount = 0;
	
	vkGetSwapchainImagesKHR(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanSwapChain, &swapChainImagesCount, nullptr);

	swapChainImages.Resize(swapChainImagesCount);
	vkGetSwapchainImagesKHR(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanSwapChain, &swapChainImagesCount, swapChainImages.Data());

	//Create the image views.
	swapChainImageViews.Resize(swapChainImagesCount);

	for (uint32 i = 0; i < swapChainImagesCount; ++i)
	{
		VulkanUtilities::CreateVulkanImageView(VulkanInterface::Instance->GetVulkanPhysicalDevice().GetSurfaceFormat().format, VK_IMAGE_ASPECT_COLOR_BIT, swapChainImages[i], swapChainImageViews[i]);
	}

	//Initialize the depth buffer.
	vulkanDepthBuffer.Initialize(swapExtent);
}

/*
*	Releases this Vulkan swap chain.
*/
void VulkanSwapchain::Release() CATALYST_NOEXCEPT
{
	//Release the depth buffer.
	vulkanDepthBuffer.Release();

	//Destroy all image views.
	for (auto &swapChainImageView : swapChainImageViews)
	{
		vkDestroyImageView(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), swapChainImageView, nullptr);
	}

	//Destroy the Vulkan swap chain.
	vkDestroySwapchainKHR(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanSwapChain, nullptr);
}

/*
*	Updates the next image index in the swap chain.
*/
void VulkanSwapchain::UpdateNextImageIndex(const VulkanSemaphore &imageAvailableSemaphore) CATALYST_NOEXCEPT
{
	vkAcquireNextImageKHR(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanSwapChain, std::numeric_limits<uint64>::max(), imageAvailableSemaphore.Get(), VK_NULL_HANDLE, &currentImageIndex);
}

/*
*	Presents the current image to the screen.
*/
void VulkanSwapchain::Present(const VulkanSemaphore &renderFinishedSemaphore, const VulkanQueue &presentQueue) CATALYST_NOEXCEPT
{
	//Create the present info.
	VkPresentInfoKHR presentInfo;
	CreatePresentInfo(presentInfo, renderFinishedSemaphore);

	//Present on the present queue!
	vkQueuePresentKHR(presentQueue.Get(), &presentInfo);
}

/*
*	Finds the most optimal swap extent.
*/
void VulkanSwapchain::FindMostOptimalSwapExtent() CATALYST_NOEXCEPT
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanInterface::Instance->GetVulkanPhysicalDevice().Get(), VulkanInterface::Instance->GetVulkanSurface().Get(), &surfaceCapabilities);

	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
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
void VulkanSwapchain::CreateSwapChainCreateInfo(VkSwapchainCreateInfoKHR &swapChainCreateInfo, uint32 *CATALYST_RESTRICT queueFamilyIndices, const uint32 graphicsQueueFamilyIndex, const uint32 presentQueueFamilyIndex) const CATALYST_NOEXCEPT
{
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.pNext = nullptr;
	swapChainCreateInfo.flags = 0;
	swapChainCreateInfo.surface = VulkanInterface::Instance->GetVulkanSurface().Get();

	const auto &surfaceCapabilities = VulkanInterface::Instance->GetVulkanPhysicalDevice().GetSurfaceCapabilities();
	const auto &surfaceFormat = VulkanInterface::Instance->GetVulkanPhysicalDevice().GetSurfaceFormat();
	const auto &presentMode = VulkanInterface::Instance->GetVulkanPhysicalDevice().GetPresentMode();

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
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode = presentMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
}

/*
*	Creates a present info.
*/
void VulkanSwapchain::CreatePresentInfo(VkPresentInfoKHR &presentInfo, const VulkanSemaphore &renderFinishedSemaphore) const CATALYST_NOEXCEPT
{
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore.Get();
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vulkanSwapChain;
	presentInfo.pImageIndices = &currentImageIndex;
	presentInfo.pResults = nullptr;
}