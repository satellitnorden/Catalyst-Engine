#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;

class VulkanDescriptorSet final
{

public:

	/*
	*	Returns the underlying Vulkan descriptor set.
	*/
	const VkDescriptorSet& Get() const NOEXCEPT { return vulkanDescriptorSet; }

	/*
	*	Initializes this Vulkan descriptor set.
	*/
	void Initialize(const VulkanDescriptorPool &vulkanDescriptorPool, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) NOEXCEPT;

private:

	//The underlying Vulkan descriptor set.
	VkDescriptorSet vulkanDescriptorSet;

	/*
	*	Creates a descriptor set allocate info.
	*/
	void CreateDescriptorSetAllocateInfo(VkDescriptorSetAllocateInfo &descriptorSetAllocateInfo, const VulkanDescriptorPool &vulkanDescriptorPool, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) const NOEXCEPT;


};