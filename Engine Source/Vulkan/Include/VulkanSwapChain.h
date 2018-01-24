#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>
#include <VulkanDepthBuffer.h>

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
	*	Default constructor.
	*/
	VulkanSwapchain() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanSwapchain() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan swap chain.
	*/
	const VULKAN_SWAPCHAIN_TYPE& Get() const CATALYST_NOEXCEPT { return vulkanSwapChain; }

	/*
	*	Initializes this Vulkan swap chain.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan swap chain.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the swap chain images.
	*/
	const DynamicArray<VkImage>& GetSwapChainImages() const CATALYST_NOEXCEPT { return swapChainImages; }

	/*
	*	Returns the swap chain image views.
	*/
	const DynamicArray<VkImageView>& GetSwapChainImageViews() const CATALYST_NOEXCEPT { return swapChainImageViews; }

	/*
	*	Returns the Vulkan depth buffer.
	*/
	const VulkanDepthBuffer& GetDepthBuffer() const CATALYST_NOEXCEPT { return vulkanDepthBuffer; }

	/*
	*	Returns the swap extent.
	*/
	VkExtent2D GetSwapExtent() const CATALYST_NOEXCEPT { return swapExtent; }

	/*
	*	Returns the current image index.
	*/
	uint32 GetCurrentImageIndex() const CATALYST_NOEXCEPT { return currentImageIndex; }

	/*
	*	Updates the next image index in the swap chain.
	*/
	void UpdateNextImageIndex(const VulkanSemaphore *const CATALYST_RESTRICT imageAvailableSemaphore) CATALYST_NOEXCEPT;

	/*
	*	Presents the current image to the screen.
	*/
	void Present(const VulkanSemaphore *const CATALYST_RESTRICT renderFinishedSemaphore) CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan swap chain.
	VULKAN_SWAPCHAIN_TYPE vulkanSwapChain;

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
	void FindMostOptimalSwapExtent() CATALYST_NOEXCEPT;

	/*
	*	Creates the swap chain create info.
	*/
	void CreateSwapChainCreateInfo(VULKAN_SWAPCHAIN_CREATE_INFO_TYPE &swapChainCreateInfo, uint32 *CATALYST_RESTRICT queueFamilyIndices, const uint32 graphicsQueueFamilyIndex, const uint32 presentQueueFamilyIndex) const CATALYST_NOEXCEPT;

	/*
	*	Creates a present info.
	*/
	void CreatePresentInfo(VULKAN_PRESENT_INFO_TYPE &presentInfo, const VulkanSemaphore *const CATALYST_RESTRICT renderFinishedSemaphore) const CATALYST_NOEXCEPT;

};