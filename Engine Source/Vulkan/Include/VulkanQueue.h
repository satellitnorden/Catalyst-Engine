#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanCommandBuffer;

class VulkanQueue
{

public:

	/*
	*	Default constructor.
	*/
	VulkanQueue() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanQueue() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan queue.
	*/
	CATALYST_FORCE_INLINE const VkQueue& Get() const CATALYST_NOEXCEPT { return vulkanQueue; }

	/*
	*	Initializes this Vulkan queue.
	*/
	void Initialize(const VkQueue &newVulkanQueue) CATALYST_NOEXCEPT;

	/*
	*	Submits a command buffer to this Vulkan queue.
	*/
	void Submit(const VulkanCommandBuffer &vulkanCommandBuffer, const DynamicArray<VkSemaphore> &waitSemaphores = DynamicArray<VkSemaphore>(), const VkPipelineStageFlags &waitStages = 0, const DynamicArray<VkSemaphore> &signalSemaphores = DynamicArray<VkSemaphore>(), const VkFence fence = VK_NULL_HANDLE) const CATALYST_NOEXCEPT;

	/*
	*	Waits idle for this Vulkan queue.
	*/
	void WaitIdle() const CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan queue.
	VkQueue vulkanQueue;

	/*
	*	Creates a submit info for a single command buffer.
	*/
	void CreateSubmitInfo(VkSubmitInfo &submitInfo, const DynamicArray<VkSemaphore> &waitSemaphores, const VkPipelineStageFlags &waitStages, const VulkanCommandBuffer &vulkanCommandBuffer, const DynamicArray<VkSemaphore> &signalSemaphores) const CATALYST_NOEXCEPT;

	/*
	*	Creates a submit info for multiple command buffers.
	*/
	void CreateSubmitInfo(VkSubmitInfo &submitInfo, const DynamicArray<VkSemaphore> &waitSemaphores, const VkPipelineStageFlags &waitStages, const DynamicArray<VulkanCommandBuffer> &vulkanCommandBuffers, const DynamicArray<VkSemaphore> &signalSemaphores) const CATALYST_NOEXCEPT;

};