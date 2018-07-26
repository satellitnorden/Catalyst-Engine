//Header file.
#include <Rendering/API Layer/Vulkan/VulkanQueue.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan queue.
*/
void VulkanQueue::Initialize(const uint32 queueFamilyIndex) NOEXCEPT
{
	//Get the Vulkan queue.
	vkGetDeviceQueue(VulkanInterface::Instance->GetLogicalDevice().Get(), queueFamilyIndex, 0, &vulkanQueue);
}

/*
*	Submits a command buffer to this Vulkan queue.
*/
void VulkanQueue::Submit(const VulkanCommandBuffer &vulkanCommandBuffer, const uint32 waitSemaphoreCount, const VulkanSemaphore *RESTRICT waitSemaphores, const VkPipelineStageFlags waitStages, const uint32 signalSemaphoreCount, const VulkanSemaphore *RESTRICT signalSemaphores, const VkFence fence) const NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ lock };

	//Create the submit info.
	VkSubmitInfo submitInfo;
	CreateSubmitInfo(submitInfo, waitSemaphoreCount, waitSemaphores, waitStages, vulkanCommandBuffer, signalSemaphoreCount, signalSemaphores);

	//Submit the command buffer!
	VULKAN_ERROR_CHECK(vkQueueSubmit(vulkanQueue, 1, &submitInfo, fence));
}

/*
*	Waits idle for this Vulkan queue.
*/
void VulkanQueue::WaitIdle() const NOEXCEPT
{
	//Lock the queue.
	ScopedLock<Spinlock> scopedLock{ lock };

	//Wait idle for this Vulkan queue.
	VULKAN_ERROR_CHECK(vkQueueWaitIdle(vulkanQueue));
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