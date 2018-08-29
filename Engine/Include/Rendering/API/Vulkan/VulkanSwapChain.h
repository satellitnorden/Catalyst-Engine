#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanDepthBuffer.h>

//Forward declarations.
class VulkanLogicalDevice;
class VulkanPhysicalDevice;
class VulkanSemaphore;
class VulkanSurface;
class VulkanQueue;

class VulkanSwapchain final
{

public:

	/*
	*	Returns the underlying Vulkan swap chain.
	*/
	const VkSwapchainKHR Get() const NOEXCEPT { return vulkanSwapChain; }

	/*
	*	Initializes this Vulkan swap chain.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Releases this Vulkan swap chain.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the number of swapchain images.
	*/
	uint32 GetNumberOfSwapChainImages() const NOEXCEPT { return numberOfSwapChainImages; }

	/*
	*	Returns the swap chain images.
	*/
	const DynamicArray<VkImage>& GetSwapChainImages() const NOEXCEPT { return swapChainImages; }

	/*
	*	Returns the swap chain image views.
	*/
	const DynamicArray<VkImageView>& GetSwapChainImageViews() const NOEXCEPT { return swapChainImageViews; }

	/*
	*	Returns the Vulkan depth buffer.
	*/
	const VulkanDepthBuffer& GetDepthBuffer() const NOEXCEPT { return vulkanDepthBuffer; }

	/*
	*	Returns the swap extent.
	*/
	VkExtent2D GetSwapExtent() const NOEXCEPT { return swapExtent; }

	/*
	*	Returns the current image index.
	*/
	uint32 GetCurrentImageIndex() const NOEXCEPT { return currentImageIndex; }

	/*
	*	Updates the next image index in the swap chain.
	*/
	void UpdateNextImageIndex(const VulkanSemaphore *const RESTRICT imageAvailableSemaphore) NOEXCEPT;

	/*
	*	Presents the current image to the screen.
	*/
	void Present(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) NOEXCEPT;

private:

	//The underlying Vulkan swap chain.
	VkSwapchainKHR vulkanSwapChain;

	//The number of swap chain images.
	uint32 numberOfSwapChainImages;

	//The swap chain images.
	DynamicArray<VkImage> swapChainImages;

	//The swap chain image views.
	DynamicArray<VkImageView> swapChainImageViews;

	//The Vulkan depth buffer.
	VulkanDepthBuffer vulkanDepthBuffer;

	//The extent of the swap chain.
	VkExtent2D swapExtent;

	//The current image index.
	uint32 currentImageIndex;

	/*
	*	Finds the most optimal swap extent.
	*/
	void FindMostOptimalSwapExtent() NOEXCEPT;

	/*
	*	Creates the swap chain create info.
	*/
	void CreateSwapChainCreateInfo(VkSwapchainCreateInfoKHR &swapChainCreateInfo, uint32 *RESTRICT queueFamilyIndices, const uint32 graphicsQueueFamilyIndex, const uint32 presentQueueFamilyIndex) const NOEXCEPT;

};