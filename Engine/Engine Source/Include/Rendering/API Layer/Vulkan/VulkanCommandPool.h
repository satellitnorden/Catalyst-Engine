#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanCommandBuffer;

class VulkanCommandPool final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanCommandPool() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanCommandPool() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan command pool.
	*/
	const VkCommandPool& Get() const NOEXCEPT { return vulkanCommandPool; }

	/*
	*	Initializes this Vulkan command pool.
	*/
	void Initialize(const uint32 queueFamilyIndex) NOEXCEPT;

	/*
	*	Releases this Vulkan command pool.
	*/
	void Release() NOEXCEPT;

	/*
	*	Allocates and returns a Vulkan command buffer.
	*/
	void AllocateVulkanCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT;

	/*
	*	Frees a Vulkan command buffer.
	*/
	void FreeVulkanCommandBuffer(VulkanCommandBuffer &vulkanCommandBuffer) const NOEXCEPT;

private:

	//The underlying Vulkan command pool.
	VkCommandPool vulkanCommandPool;

	/*
	*	Creates a command pool create info.
	*/
	void CreateCommandPoolCreateInfo(VkCommandPoolCreateInfo &commandPoolCreateInfo, const uint32 queueFamilyIndex) const NOEXCEPT;

};