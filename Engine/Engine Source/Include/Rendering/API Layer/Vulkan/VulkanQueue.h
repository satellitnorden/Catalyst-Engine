#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanCommandBuffer;
class VulkanSemaphore;

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
	void Submit(const VulkanCommandBuffer &vulkanCommandBuffer, const uint32 waitSemaphoreCount, const VulkanSemaphore *RESTRICT waitSemaphores, const VkPipelineStageFlags waitStages, const uint32 signalSemaphoreCount, const VulkanSemaphore *RESTRICT signalSemaphores, const VkFence fence) const NOEXCEPT;

	/*
	*	Waits idle for this Vulkan queue.
	*/
	void WaitIdle() const NOEXCEPT;

private:

	//The underlying Vulkan queue.
	VkQueue vulkanQueue;

	//The lock for the queue.
	mutable Spinlock lock;

	/*
	*	Creates a submit info for a single command buffer.
	*/
	void CreateSubmitInfo(VkSubmitInfo &submitInfo, const uint32 waitSemaphoreCount, const VulkanSemaphore *RESTRICT waitSemaphores, const VkPipelineStageFlags &waitStages, const VulkanCommandBuffer &vulkanCommandBuffer, const uint32 signalSemaphoreCount, const VulkanSemaphore *RESTRICT signalSemaphores) const NOEXCEPT;

};