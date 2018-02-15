//Header file.
#include <VulkanQueue.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanQueue::VulkanQueue() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanQueue::~VulkanQueue() NOEXCEPT
{

}

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
void VulkanQueue::Submit(const VulkanCommandBuffer &vulkanCommandBuffer, const DynamicArray<VkSemaphore> &waitSemaphores, const VkPipelineStageFlags &waitStages, const DynamicArray<VkSemaphore> &signalSemaphores, const VkFence fence) const NOEXCEPT
{
	//Create the submit info.
	VkSubmitInfo submitInfo;
	CreateSubmitInfo(submitInfo, waitSemaphores, waitStages, vulkanCommandBuffer, signalSemaphores);

	//Submit the command buffer!
	VULKAN_ERROR_CHECK(vkQueueSubmit(vulkanQueue, 1, &submitInfo, fence));
}

/*
*	Waits idle for this Vulkan queue.
*/
void VulkanQueue::WaitIdle() const NOEXCEPT
{
	//Wait idle for this Vulkan queue.
	VULKAN_ERROR_CHECK(vkQueueWaitIdle(vulkanQueue));
}

/*
*	Creates a submit info for a single command buffer.
*/
void VulkanQueue::CreateSubmitInfo(VkSubmitInfo &submitInfo, const DynamicArray<VkSemaphore> &waitSemaphores, const VkPipelineStageFlags &waitStages, const VulkanCommandBuffer &vulkanCommandBuffer, const DynamicArray<VkSemaphore> &signalSemaphores) const NOEXCEPT
{
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = static_cast<uint32>(waitSemaphores.Size());
	submitInfo.pWaitSemaphores = waitSemaphores.Data();
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkanCommandBuffer.Get();
	submitInfo.signalSemaphoreCount = static_cast<uint32>(signalSemaphores.Size());
	submitInfo.pSignalSemaphores = signalSemaphores.Data();
}

/*
*	Creates a submit info for multiple command buffers.
*/
void VulkanQueue::CreateSubmitInfo(VkSubmitInfo &submitInfo, const DynamicArray<VkSemaphore> &waitSemaphores, const VkPipelineStageFlags &waitStages, const DynamicArray<VulkanCommandBuffer> &vulkanCommandBuffers, const DynamicArray<VkSemaphore> &signalSemaphores) const NOEXCEPT
{
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = static_cast<uint32>(waitSemaphores.Size());
	submitInfo.pWaitSemaphores = waitSemaphores.Data();
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = static_cast<uint32>(vulkanCommandBuffers.Size());
	submitInfo.pCommandBuffers = reinterpret_cast<const VkCommandBuffer*>(vulkanCommandBuffers.Data());
	submitInfo.signalSemaphoreCount = static_cast<uint32>(signalSemaphores.Size());
	submitInfo.pSignalSemaphores = signalSemaphores.Data();
}