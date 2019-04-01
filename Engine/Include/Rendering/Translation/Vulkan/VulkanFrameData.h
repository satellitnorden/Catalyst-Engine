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
	void Initialize(const uint32 frameDataCount) NOEXCEPT
	{
		//Create the image available semaphore.
		_ImageAvailableSemaphore = VulkanInterface::Instance->CreateSemaphore();

		//Create the render finished semaphore.
		_RenderFinishedSemaphore = VulkanInterface::Instance->CreateSemaphore();

		//Create the primary command pool.
		_PrimaryCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the primary command buffers.
		_PrimaryCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &primaryCommandBuffer : _PrimaryCommandBuffers)
		{
			_PrimaryCommandPool->AllocatePrimaryCommandBuffer(primaryCommandBuffer);
		}

		//Create the fences.
		_Fences.UpsizeFast(frameDataCount);

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
		return _ImageAvailableSemaphore;
	}

	/*
	*	Returns the render finished semaphore.
	*/
	RESTRICTED NO_DISCARD VulkanSemaphore *const RESTRICT GetRenderFinishedSemaphore() NOEXCEPT
	{
		return _RenderFinishedSemaphore;
	}

	/*
	*	Returns the current frame.
	*/
	uint32 GetCurrentFrame() const NOEXCEPT
	{
		return _CurrentFrame;
	}

	/*
	*	Sets the current frame.
	*/
	void SetCurrentFrame(const uint32 newCurrentFrame) NOEXCEPT
	{
		_CurrentFrame = newCurrentFrame;
	}

	/*
	*	Returns the current primary command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentPrimaryCommandBuffer() NOEXCEPT { return &_PrimaryCommandBuffers[_CurrentFrame]; }

	/*
	*	Returns the current fence.
	*/
	VulkanFence *RESTRICT GetCurrentFence() NOEXCEPT
	{
		//Return the current fence.
		return _Fences[_CurrentFrame];
	}

private:

	//The image available semaphore.
	VulkanSemaphore *RESTRICT _ImageAvailableSemaphore;

	//The render finished semaphore.
	VulkanSemaphore *RESTRICT _RenderFinishedSemaphore;

	//Keeps track of the current frame.
	uint32 _CurrentFrame;

	//The primary command pool.
	VulkanCommandPool *RESTRICT _PrimaryCommandPool;

	//The primary command buffers.
	DynamicArray<VulkanCommandBuffer> _PrimaryCommandBuffers;

	//The fences.
	DynamicArray<VulkanFence *RESTRICT> _Fences;

};
#endif