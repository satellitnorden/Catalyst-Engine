#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;

class VulkanDescriptorSet final
{

public:

	/*
	*	Returns the underlying Vulkan descriptor set.
	*/
	const VkDescriptorSet& Get() const NOEXCEPT { return _VulkanDescriptorSet; }

	/*
	*	Initializes this Vulkan descriptor set.
	*/
	void Initialize(const VulkanDescriptorPool &vulkanDescriptorPool, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) NOEXCEPT;

private:

	//The underlying Vulkan descriptor set.
	VkDescriptorSet _VulkanDescriptorSet;

	/*
	*	Creates a descriptor set allocate info.
	*/
	void CreateDescriptorSetAllocateInfo(VkDescriptorSetAllocateInfo &descriptorSetAllocateInfo, const VulkanDescriptorPool &vulkanDescriptorPool, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) const NOEXCEPT;


};
#endif