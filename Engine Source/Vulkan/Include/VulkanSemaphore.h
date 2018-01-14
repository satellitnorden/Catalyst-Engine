#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanLogicalDevice;

class VulkanSemaphore
{

public:

	/*
	*	Default constructor.
	*/
	VulkanSemaphore() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanSemaphore() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan semaphore.
	*/
	const VkSemaphore& Get() const CATALYST_NOEXCEPT { return vulkanSemaphore; }

	/*
	*	Initializes this Vulkan semaphore.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan semaphore.
	*/
	void Release() CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan semaphore.
	VkSemaphore vulkanSemaphore;

	/*
	*	Creates a semaphore create info.
	*/
	void CreateSemaphoreCreateInfo(VkSemaphoreCreateInfo &semaphoreCreateInfo) const CATALYST_NOEXCEPT;

};