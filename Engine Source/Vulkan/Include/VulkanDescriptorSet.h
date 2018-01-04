#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;

class VulkanDescriptorSet
{

public:

	/*
	*	Default constructor.
	*/
	VulkanDescriptorSet() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanDescriptorSet() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan descriptor set.
	*/
	CATALYST_FORCE_INLINE const VkDescriptorSet& Get() const CATALYST_NOEXCEPT { return vulkanDescriptorSet; }

	/*
	*	Initializes this Vulkan descriptor set.
	*/
	void Initialize(const VulkanDescriptorPool &vulkanDescriptorPool, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan descriptor set.
	VkDescriptorSet vulkanDescriptorSet;

	/*
	*	Creates a descriptor set allocate info.
	*/
	void CreateDescriptorSetAllocateInfo(VkDescriptorSetAllocateInfo &descriptorSetAllocateInfo, const VulkanDescriptorPool &vulkanDescriptorPool, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) const CATALYST_NOEXCEPT;


};