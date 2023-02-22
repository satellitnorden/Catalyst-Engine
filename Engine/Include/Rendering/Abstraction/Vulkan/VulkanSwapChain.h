#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanSemaphore;

class VulkanSwapchain final
{

public:

	/*
	*	Returns the underlying Vulkan swap chain.
	*/
	const VkSwapchainKHR Get() const NOEXCEPT
	{
		return _VulkanSwapchain;
	}

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
	FORCE_INLINE uint32 GetNumberOfSwapchainImages() const NOEXCEPT
	{
		return _NumberOfSwapchainImages;
	}

	/*
	*	Sets the number of swapchain images.
	*/
	FORCE_INLINE void SetNumberOfSwapchainImages(const uint32 value) NOEXCEPT
	{
		_NumberOfSwapchainImages = value;
	}

	/*
	*	Returns the swap chain images.
	*/
	FORCE_INLINE const DynamicArray<VkImage> &GetSwapchainImages() const NOEXCEPT
	{
		return _SwapchainImages;
	}

	/*
	*	Adds a swapchain image.
	*/
	FORCE_INLINE void AddSwapchainImage(const VkImage new_image) NOEXCEPT
	{
		_SwapchainImages.Emplace(new_image);
	}

	/*
	*	Returns the swap chain image views.
	*/
	FORCE_INLINE const DynamicArray<VkImageView> &GetSwapchainImageViews() const NOEXCEPT
	{
		return _SwapchainImageViews;
	}

	/*
	*	Adds a swapchain image view.
	*/
	FORCE_INLINE void AddSwapchainImageView(const VkImageView new_image_view) NOEXCEPT
	{
		_SwapchainImageViews.Emplace(new_image_view);
	}

	/*
	*	Returns the swap extent.
	*/
	FORCE_INLINE VkExtent2D GetSwapExtent() const NOEXCEPT
	{
		return _SwapExtent;
	}

	/*
	*	Sets the swap extent.
	*/
	FORCE_INLINE void SetSwapExtent(const VkExtent2D value) NOEXCEPT
	{
		_SwapExtent = value;
	}

	/*
	*	Returns the current image index.
	*/
	FORCE_INLINE uint32 GetCurrentImageIndex() const NOEXCEPT
	{
		return _CurrentImageIndex;
	}

	/*
	*	Sets the current image index.
	*/
	FORCE_INLINE void SetCurrentImageIndex(const uint32 value) NOEXCEPT
	{
		_CurrentImageIndex = value;
	}

	/*
	*	Updates the next image index in the swap chain.
	*/
	void UpdateNextImageIndex(const VulkanSemaphore *const RESTRICT imageAvailableSemaphore) NOEXCEPT;

	/*
	*	Presents the current image to the screen.
	*/
	void Present(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) NOEXCEPT;

private:

	//The underlying Vulkan swapchain.
	VkSwapchainKHR _VulkanSwapchain{ VK_NULL_HANDLE };

	//The number of swapchain images.
	uint32 _NumberOfSwapchainImages;

	//The swapchain images.
	DynamicArray<VkImage> _SwapchainImages;

	//The swapchain image views.
	DynamicArray<VkImageView> _SwapchainImageViews;

	//The extent of the swapchain.
	VkExtent2D _SwapExtent;

	//The current image index.
	uint32 _CurrentImageIndex{ 0 };

	/*
	*	Finds the most optimal swap extent.
	*/
	void FindMostOptimalSwapExtent() NOEXCEPT;

	/*
	*	Creates the swap chain create info.
	*/
	void CreateSwapChainCreateInfo(VkSwapchainCreateInfoKHR &swapChainCreateInfo) const NOEXCEPT;

};
#endif