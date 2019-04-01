#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanDepthBuffer.h>

//Forward declarations.
class VulkanSemaphore;

class VulkanSwapchain final
{

public:

	/*
	*	Returns the underlying Vulkan swap chain.
	*/
	const VkSwapchainKHR Get() const NOEXCEPT { return _VulkanSwapChain; }

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
	uint32 GetNumberOfSwapChainImages() const NOEXCEPT { return _NumberOfSwapChainImages; }

	/*
	*	Returns the swap chain images.
	*/
	const DynamicArray<VkImage>& GetSwapChainImages() const NOEXCEPT { return _SwapChainImages; }

	/*
	*	Returns the swap chain image views.
	*/
	const DynamicArray<VkImageView>& GetSwapChainImageViews() const NOEXCEPT { return _SwapChainImageViews; }

	/*
	*	Returns the swap extent.
	*/
	VkExtent2D GetSwapExtent() const NOEXCEPT { return _SwapExtent; }

	/*
	*	Returns the current image index.
	*/
	uint32 GetCurrentImageIndex() const NOEXCEPT { return _CurrentImageIndex; }

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
	VkSwapchainKHR _VulkanSwapChain;

	//The number of swap chain images.
	uint32 _NumberOfSwapChainImages;

	//The swap chain images.
	DynamicArray<VkImage> _SwapChainImages;

	//The swap chain image views.
	DynamicArray<VkImageView> _SwapChainImageViews;

	//The extent of the swap chain.
	VkExtent2D _SwapExtent;

	//The current image index.
	uint32 _CurrentImageIndex;

	/*
	*	Finds the most optimal swap extent.
	*/
	void FindMostOptimalSwapExtent() NOEXCEPT;

	/*
	*	Creates the swap chain create info.
	*/
	void CreateSwapChainCreateInfo(VkSwapchainCreateInfoKHR &swapChainCreateInfo, uint32 *RESTRICT queueFamilyIndices, const uint32 graphicsQueueFamilyIndex, const uint32 presentQueueFamilyIndex) const NOEXCEPT;

};
#endif