#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanLogicalDevice;

class VulkanSemaphore final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanSemaphore() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanSemaphore() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan semaphore.
	*/
	const VkSemaphore& Get() const NOEXCEPT { return vulkanSemaphore; }

	/*
	*	Initializes this Vulkan semaphore.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Releases this Vulkan semaphore.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan semaphore.
	VkSemaphore vulkanSemaphore;

	/*
	*	Creates a semaphore create info.
	*/
	void CreateSemaphoreCreateInfo(VkSemaphoreCreateInfo &semaphoreCreateInfo) const NOEXCEPT;

};