#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanDescriptorSetLayout final
{

public:

	/*
	*	Returns the underlying Vulkan descriptor set layout.
	*/
	const VkDescriptorSetLayout& Get() const NOEXCEPT { return _VulkanDescriptorSetLayout; }

	/*
	*	Initializes this Vulkan descriptor set layout.
	*/
	void Initialize(const uint32 descriptorSetLayoutBindingCount, const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings) NOEXCEPT;

	/*
	*	Releases this Vulkan descriptor set layout.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan descriptor set layout.
	VkDescriptorSetLayout _VulkanDescriptorSetLayout;

	/*
	*	Creates a descriptor set layout create info.
	*/
	void CreateDescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo, const uint32 descriptorSetLayoutBindingCount, const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings) const NOEXCEPT;

};
#endif