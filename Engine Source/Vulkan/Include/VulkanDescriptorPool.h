#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanLogicalDevice;

class VulkanDescriptorPool
{

public:

	/*
	*	Default constructor.
	*/
	VulkanDescriptorPool() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanDescriptorPool() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan descriptor pool.
	*/
	CATALYST_FORCE_INLINE const VkDescriptorPool& Get() const CATALYST_NOEXCEPT { return vulkanDescriptorPool; }

	/*
	*	Initializes this Vulkan descriptor pool.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan descriptor pool.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Allocates a Vulkan descriptor set.
	*/
	void AllocateDescriptorSet(VulkanDescriptorSet &vulkaDescriptorSet, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) const CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan descriptor pool.
	VkDescriptorPool vulkanDescriptorPool;

	/*
	*	Creates a descriptor pool size.
	*/
	void CreateDescriptorPoolSizes(DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const CATALYST_NOEXCEPT;

	/*
	*	Creates a descriptor pool create info.
	*/
	void CreateDescriptorPoolCreateInfo(VkDescriptorPoolCreateInfo &descriptorPoolCreateInfo, const DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const CATALYST_NOEXCEPT;

};