#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanCommandBuffer;

class VulkanQueue final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanQueue() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanQueue() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan queue.
	*/
	const VkQueue& Get() const NOEXCEPT { return vulkanQueue; }

	/*
	*	Initializes this Vulkan queue.
	*/
	void Initialize(const uint32 queueFamilyIndex) NOEXCEPT;

	/*
	*	Submits a command buffer to this Vulkan queue.
	*/
	void Submit(const VulkanCommandBuffer &vulkanCommandBuffer, const DynamicArray<VkSemaphore> &waitSemaphores = DynamicArray<VkSemaphore>(), const VkPipelineStageFlags &waitStages = 0, const DynamicArray<VkSemaphore> &signalSemaphores = DynamicArray<VkSemaphore>(), const VkFence fence = VK_NULL_HANDLE) const NOEXCEPT;

	/*
	*	Waits idle for this Vulkan queue.
	*/
	void WaitIdle() const NOEXCEPT;

private:

	//The underlying Vulkan queue.
	VkQueue vulkanQueue;

	/*
	*	Creates a submit info for a single command buffer.
	*/
	void CreateSubmitInfo(VkSubmitInfo &submitInfo, const DynamicArray<VkSemaphore> &waitSemaphores, const VkPipelineStageFlags &waitStages, const VulkanCommandBuffer &vulkanCommandBuffer, const DynamicArray<VkSemaphore> &signalSemaphores) const NOEXCEPT;

	/*
	*	Creates a submit info for multiple command buffers.
	*/
	void CreateSubmitInfo(VkSubmitInfo &submitInfo, const DynamicArray<VkSemaphore> &waitSemaphores, const VkPipelineStageFlags &waitStages, const DynamicArray<VulkanCommandBuffer> &vulkanCommandBuffers, const DynamicArray<VkSemaphore> &signalSemaphores) const NOEXCEPT;

};