#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanSemaphore final
{

public:

	/*
	*	Returns the underlying Vulkan semaphore.
	*/
	const VkSemaphore& Get() const NOEXCEPT { return _VulkanSemaphore; }

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
	VkSemaphore _VulkanSemaphore;

	/*
	*	Creates a semaphore create info.
	*/
	void CreateSemaphoreCreateInfo(VkSemaphoreCreateInfo &semaphoreCreateInfo) const NOEXCEPT;

};
#endif