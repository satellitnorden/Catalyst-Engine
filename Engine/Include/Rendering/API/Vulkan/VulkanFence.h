#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanFence final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanFence() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanFence() NOEXCEPT;

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