#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanCommandBuffer;
class VulkanSemaphore;

class VulkanQueue final
{

public:

	/*
	*	Returns the underlying Vulkan queue.
	*/
	const VkQueue& Get() const NOEXCEPT { return _VulkanQueue; }

	/*
	*	Initializes this Vulkan queue.
	*/
	void Initialize(const uint32 queue_family_index, const uint32 queue_index) NOEXCEPT;

	/*
	*	Submits a command buffer to this Vulkan queue.
	*/
	void Submit(const VulkanCommandBuffer &vulkanCommandBuffer, const uint32 waitSemaphoreCount, const VulkanSemaphore *RESTRICT waitSemaphores, const VkPipelineStageFlags waitStages, const uint32 signalSemaphoreCount, const VulkanSemaphore *RESTRICT signalSemaphores, const VkFence fence) NOEXCEPT;

	/*
	*	Presents on this queue.
	*/
	void Present(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore, const VkSwapchainKHR *const RESTRICT swapchain, const uint32 *const RESTRICT imageIndex) NOEXCEPT;

	/*
	*	Waits idle for this Vulkan queue.
	*/
	void WaitIdle() const NOEXCEPT;

private:

	//The underlying Vulkan queue.
	VkQueue _VulkanQueue;

	//The lock for the queue.
	mutable Spinlock _Lock;

	/*
	*	Creates a submit info for a single command buffer.
	*/
	void CreateSubmitInfo(VkSubmitInfo &submitInfo, const uint32 waitSemaphoreCount, const VulkanSemaphore *RESTRICT waitSemaphores, const VkPipelineStageFlags &waitStages, const VulkanCommandBuffer &vulkanCommandBuffer, const uint32 signalSemaphoreCount, const VulkanSemaphore *RESTRICT signalSemaphores) const NOEXCEPT;

	/*
	*	Creates a present info.
	*/
	void CreatePresentInfo(VkPresentInfoKHR &presentInfo, const VulkanSemaphore *const RESTRICT renderFinishedSemaphore, const VkSwapchainKHR *const RESTRICT swapchain, const uint32 *const RESTRICT imageIndex) const NOEXCEPT;

};
#endif