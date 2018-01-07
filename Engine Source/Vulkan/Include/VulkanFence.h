#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

class VulkanFence
{

public:

	/*
	*	Default constructor.
	*/
	VulkanFence() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanFence() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan fence.
	*/
	const VkFence& Get() const CATALYST_NOEXCEPT { return vulkanFence; }

	/*
	*	Initializes this Vulkan fence.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan fence.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Resets this Vulkan fence.
	*/
	void Reset() CATALYST_NOEXCEPT;

	/*
	*	Waits for this Vulkan fence.
	*/
	void WaitFor() const CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan fence.
	VkFence vulkanFence{ nullptr };

	/*
	*	Creates a fence create info.
	*/
	void CreateFenceCreateInfo(VkFenceCreateInfo &fenceCreateInfo) const CATALYST_NOEXCEPT;

};