//Header file.
#include <Rendering/API/Vulkan/VulkanSwapChain.h>

//Math.
#include <Math/CatalystMath.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>
#include <Rendering/API/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/API/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/API/Vulkan/VulkanSemaphore.h>
#include <Rendering/API/Vulkan/VulkanSurface.h>
#include <Rendering/API/Vulkan/VulkanUtilities.h>

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
	const uint32 graphicsQueueFamilyIndex = VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Graphics);
	const uint32 presentQueueFamilyIndex = VulkanInterface::Instance->GetLogicalDevice().GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Present);

	uint32 queueFamilyIndices[] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

	//Create the swap chain create info.
	VULKAN_SWAPCHAIN_CREATE_INFO_TYPE swapChainCreateInfo;
	CreateSwapChainCreateInfo(swapChainCreateInfo, queueFamilyIndices, graphicsQueueFamilyIndex, presentQueueFamilyIndex);

	//Create the swap chain!
	VULKAN_ERROR_CHECK(VULKAN_CREATE_SWAPCHAIN(VulkanInterface::Instance->GetLogicalDevice().Get(), &swapChainCreateInfo, nullptr, &vulkanSwapChain));

	//Query the swap chain images.
	VULKAN_ERROR_CHECK(VULKAN_GET_SWAPCHAIN_IMAGES(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanSwapChain, &numberOfSwapChainImages, nullptr));

	swapChainImages.UpsizeFast(numberOfSwapChainImages);
	VULKAN_ERROR_CHECK(VULKAN_GET_SWAPCHAIN_IMAGES(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanSwapChain, &numberOfSwapChainImages, swapChainImages.Data()));

	//Create the image views.
	swapChainImageViews.UpsizeFast(numberOfSwapChainImages);

	for (uint32 i = 0; i < numberOfSwapChainImages; ++i)
	{
		VulkanUtilities::CreateVulkanImageView(swapChainImages[i], VK_IMAGE_VIEW_TYPE_2D, VulkanInterface::Instance->GetPhysicalDevice().GetSurfaceFormat().format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, swapChainImageViews[i]);
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
	//Present on the present queue!
	VulkanInterface::Instance->GetPresentQueue()->Present(renderFinishedSemaphore, &vulkanSwapChain, &currentImageIndex);
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

	uint32 minimumImageCount = CatalystMath::Maximum<uint32>(3, surfaceCapabilities.minImageCount);

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