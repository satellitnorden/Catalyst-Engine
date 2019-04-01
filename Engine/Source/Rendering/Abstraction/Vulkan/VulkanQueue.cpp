#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanQueue.h>

//Multithreading.
#include <Multithreading/ScopedWriteLock.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan queue.
*/
void VulkanQueue::Initialize(const uint32 queueFamilyIndex) NOEXCEPT
{
	//Get the Vulkan queue.
	vkGetDeviceQueue(VulkanInterface::Instance->GetLogicalDevice().Get(), queueFamilyIndex, 0, &_VulkanQueue);
}

/*
*	Submits a command buffer to this Vulkan queue.
*/
void VulkanQueue::Submit(const VulkanCommandBuffer &vulkanCommandBuffer, const uint32 waitSemaphoreCount, const VulkanSemaphore *RESTRICT waitSemaphores, const VkPipelineStageFlags waitStages, const uint32 signalSemaphoreCount, const VulkanSemaphore *RESTRICT signalSemaphores, const VkFence fence) NOEXCEPT
{
	//Create the submit info.
	VkSubmitInfo submitInfo;
	CreateSubmitInfo(submitInfo, waitSemaphoreCount, waitSemaphores, waitStages, vulkanCommandBuffer, signalSemaphoreCount, signalSemaphores);

	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _Lock };

	//Submit the command buffer!
	VULKAN_ERROR_CHECK(vkQueueSubmit(_VulkanQueue, 1, &submitInfo, fence));
}

/*
*	Presents on this queue.
*/
void VulkanQueue::Present(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore, const VkSwapchainKHR *const RESTRICT swapchain, const uint32 *const RESTRICT imageIndex) NOEXCEPT
{
	//Create the present info.
	VkPresentInfoKHR presentInfo;
	CreatePresentInfo(presentInfo, renderFinishedSemaphore, swapchain, imageIndex);

	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _Lock };

	//Present!
	VULKAN_ERROR_CHECK(vkQueuePresentKHR(_VulkanQueue, &presentInfo));
}

/*
*	Waits idle for this Vulkan queue.
*/
void VulkanQueue::WaitIdle() const NOEXCEPT
{
	//Lock the queue.
	ScopedWriteLock<Spinlock> scopedLock{ _Lock };

	//Wait idle for this Vulkan queue.
	VULKAN_ERROR_CHECK(vkQueueWaitIdle(_VulkanQueue));
}

/*
*	Creates a submit info for a single command buffer.
*/
void VulkanQueue::CreateSubmitInfo(VkSubmitInfo &submitInfo, const uint32 waitSemaphoreCount, const VulkanSemaphore *RESTRICT waitSemaphores, const VkPipelineStageFlags &waitStages, const VulkanCommandBuffer &vulkanCommandBuffer, const uint32 signalSemaphoreCount, const VulkanSemaphore *RESTRICT signalSemaphores) const NOEXCEPT
{
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = waitSemaphoreCount;
	submitInfo.pWaitSemaphores = reinterpret_cast<const VkSemaphore *RESTRICT>(waitSemaphores);
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkanCommandBuffer.Get();
	submitInfo.signalSemaphoreCount = signalSemaphoreCount;
	submitInfo.pSignalSemaphores = reinterpret_cast<const VkSemaphore *RESTRICT>(signalSemaphores);
}

/*
*	Creates a present info.
*/
void VulkanQueue::CreatePresentInfo(VkPresentInfoKHR &presentInfo, const VulkanSemaphore *const RESTRICT renderFinishedSemaphore, const VkSwapchainKHR *const RESTRICT swapchain, const uint32 *const RESTRICT imageIndex) const NOEXCEPT
{
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = reinterpret_cast<const VkSemaphore *RESTRICT>(renderFinishedSemaphore);
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchain;
	presentInfo.pImageIndices = imageIndex;
	presentInfo.pResults = nullptr;
}
#endif