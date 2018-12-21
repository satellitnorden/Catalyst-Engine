#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanLogicalDevice;

class VulkanDescriptorPool final
{

public:

	/*
	*	Returns the underlying Vulkan descriptor pool.
	*/
	const VkDescriptorPool& Get() const NOEXCEPT { return _VulkanDescriptorPool; }

	/*
	*	Initializes this Vulkan descriptor pool.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Releases this Vulkan descriptor pool.
	*/
	void Release() NOEXCEPT;

	/*
	*	Allocates a Vulkan descriptor set.
	*/
	void AllocateDescriptorSet(VulkanDescriptorSet &vulkaDescriptorSet, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) const NOEXCEPT;

	/*
	*	Frees a Vulkan descritor set.
	*/
	void FreeDescriptorSet(VkDescriptorSet descriptorSet) const NOEXCEPT;

private:

	//The underlying Vulkan descriptor pool.
	VkDescriptorPool _VulkanDescriptorPool;

	//The lock for the descriptor pool.
	mutable Spinlock _Lock;

	/*
	*	Creates a descriptor pool size.
	*/
	void CreateDescriptorPoolSizes(DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const NOEXCEPT;

	/*
	*	Creates a descriptor pool create info.
	*/
	void CreateDescriptorPoolCreateInfo(VkDescriptorPoolCreateInfo &descriptorPoolCreateInfo, const DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const NOEXCEPT;

};
#endif