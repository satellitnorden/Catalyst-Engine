#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanCommandBuffer;

class VulkanCommandPool
{

public:

	/*
	*	Default constructor.
	*/
	VulkanCommandPool() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanCommandPool() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan command pool.
	*/
	const VkCommandPool& Get() const CATALYST_NOEXCEPT { return vulkanCommandPool; }

	/*
	*	Initializes this Vulkan command pool.
	*/
	void Initialize(const uint32 queueFamilyIndex) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan command pool.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Allocates and returns a Vulkan command buffer.
	*/
	void AllocateVulkanCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const CATALYST_NOEXCEPT;

	/*
	*	Frees a Vulkan command buffer.
	*/
	void FreeVulkanCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan command pool.
	VkCommandPool vulkanCommandPool;

	/*
	*	Creates a command pool create info.
	*/
	void CreateCommandPoolCreateInfo(VkCommandPoolCreateInfo &commandPoolCreateInfo, const uint32 queueFamilyIndex) const CATALYST_NOEXCEPT;

};