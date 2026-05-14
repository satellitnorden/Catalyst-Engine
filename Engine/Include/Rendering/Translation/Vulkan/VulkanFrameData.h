#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

class VulkanFrameData final
{

public:

	/*
	*	Initializes the Vulkan frame data.
	*/
	void Initialize(const uint32 number_of_frames) NOEXCEPT
	{
		//Create the image available semaphores.
		_ImageAvailableSemaphores.Upsize<false>(number_of_frames);

		for (VulkanSemaphore *RESTRICT &image_available_semaphore : _ImageAvailableSemaphores)
		{
			image_available_semaphore = VulkanInterface::Instance->CreateSemaphore();
		}

		//Create the render finished semaphores.
		_RenderFinishedSemaphores.Upsize<false>(number_of_frames);

		for (VulkanSemaphore *RESTRICT &render_finished_semaphore : _RenderFinishedSemaphores)
		{
			render_finished_semaphore = VulkanInterface::Instance->CreateSemaphore();
		}

		//Create the fences.
		_Fences.Upsize<false>(number_of_frames);

		for (VulkanFence *RESTRICT &fence : _Fences)
		{
			fence = VulkanInterface::Instance->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
		}
	}

	/*
	*	Returns the image available semaphore.
	*/
	RESTRICTED NO_DISCARD VulkanSemaphore *const RESTRICT GetImageAvailableSemaphore() NOEXCEPT
	{
		return _ImageAvailableSemaphores[_CurrentFrameIndex];
	}

	/*
	*	Returns the render finished semaphore.
	*/
	RESTRICTED NO_DISCARD VulkanSemaphore *const RESTRICT GetRenderFinishedSemaphore() NOEXCEPT
	{
		return _RenderFinishedSemaphores[_CurrentSwapchainIndex];
	}

	/*
	*	Returns the current fence.
	*/
	RESTRICTED NO_DISCARD VulkanFence *RESTRICT GetCurrentFence() NOEXCEPT
	{
		//Return the current fence.
		return _Fences[_CurrentFrameIndex];
	}

	/*
	*	Sets the current frame index.
	*/
	FORCE_INLINE void SetCurrentFrameIndex(const uint32 value) NOEXCEPT
	{
		_CurrentFrameIndex = value;
	}

	/*
	*	Sets the current swapchain index.
	*/
	FORCE_INLINE void SetCurrentSwapchainIndex(const uint32 value) NOEXCEPT
	{
		_CurrentSwapchainIndex = value;
	}

private:

	//The image available semaphores.
	DynamicArray<VulkanSemaphore *RESTRICT> _ImageAvailableSemaphores;

	//The render finished semaphore.
	DynamicArray<VulkanSemaphore *RESTRICT> _RenderFinishedSemaphores;

	//The fences.
	DynamicArray<VulkanFence* RESTRICT> _Fences;

	//Keeps track of the current frame index.
	uint32 _CurrentFrameIndex{ 0 };

	//Keeps track of the current swapchain index.
	uint32 _CurrentSwapchainIndex{ 0 };

};
#endif