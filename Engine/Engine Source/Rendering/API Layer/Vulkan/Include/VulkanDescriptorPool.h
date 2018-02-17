#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanLogicalDevice;

class VulkanDescriptorPool final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanDescriptorPool() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanDescriptorPool() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan descriptor pool.
	*/
	const VkDescriptorPool& Get() const NOEXCEPT { return vulkanDescriptorPool; }

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

private:

	//The underlying Vulkan descriptor pool.
	VkDescriptorPool vulkanDescriptorPool;

	/*
	*	Creates a descriptor pool size.
	*/
	void CreateDescriptorPoolSizes(DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const NOEXCEPT;

	/*
	*	Creates a descriptor pool create info.
	*/
	void CreateDescriptorPoolCreateInfo(VkDescriptorPoolCreateInfo &descriptorPoolCreateInfo, const DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const NOEXCEPT;

};