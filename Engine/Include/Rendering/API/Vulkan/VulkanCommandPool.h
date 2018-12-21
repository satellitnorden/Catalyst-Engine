#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanCommandBuffer;

class VulkanCommandPool final
{

public:

	/*
	*	Returns the underlying Vulkan command pool.
	*/
	const VkCommandPool& Get() const NOEXCEPT { return _VulkanCommandPool; }

	/*
	*	Initializes this Vulkan command pool.
	*/
	void Initialize(const VkCommandPoolCreateFlags flags, const uint32 queueFamilyIndex) NOEXCEPT;

	/*
	*	Releases this Vulkan command pool.
	*/
	void Release() NOEXCEPT;

	/*
	*	Allocates and returns a primary command buffer.
	*/
	void AllocatePrimaryCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT;

	/*
	*	Allocates and returns a secondary command buffer.
	*/
	void AllocateSecondaryCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT;

	/*
	*	Frees a command buffer.
	*/
	void FreeCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT;

private:

	//The underlying Vulkan command pool.
	VkCommandPool _VulkanCommandPool;

	/*
	*	Creates a command pool create info.
	*/
	void CreateCommandPoolCreateInfo(VkCommandPoolCreateInfo &commandPoolCreateInfo, const VkCommandPoolCreateFlags flags, const uint32 queueFamilyIndex) const NOEXCEPT;

};
#endif