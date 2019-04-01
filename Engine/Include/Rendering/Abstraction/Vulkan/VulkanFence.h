#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanFence final
{

public:

	/*
	*	Returns the underlying Vulkan fence.
	*/
	const VkFence& Get() const NOEXCEPT { return _VulkanFence; }

	/*
	*	Initializes this Vulkan fence.
	*/
	void Initialize(const VkFenceCreateFlags flags) NOEXCEPT;

	/*
	*	Releases this Vulkan fence.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns whether or not this fence is signaled.
	*/
	bool IsSignaled() const NOEXCEPT;

	/*
	*	Resets this Vulkan fence.
	*/
	void Reset() NOEXCEPT;

	/*
	*	Waits for this Vulkan fence.
	*/
	void WaitFor() const NOEXCEPT;

private:

	//The underlying Vulkan fence.
	VkFence _VulkanFence;

	/*
	*	Creates a fence create info.
	*/
	void CreateFenceCreateInfo(VkFenceCreateInfo &fenceCreateInfo, const VkFenceCreateFlags flags) const NOEXCEPT;

};
#endif