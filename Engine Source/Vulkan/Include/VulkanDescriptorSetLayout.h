#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

class VulkanDescriptorSetLayout final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanDescriptorSetLayout() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanDescriptorSetLayout() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan descriptor set layout.
	*/
	const VkDescriptorSetLayout& Get() const CATALYST_NOEXCEPT { return vulkanDescriptorSetLayout; }

	/*
	*	Initializes this Vulkan descriptor set layout.
	*/
	void Initialize(const DynamicArray<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan descriptor set layout.
	*/
	void Release() CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan descriptor set layout.
	VkDescriptorSetLayout vulkanDescriptorSetLayout;

	/*
	*	Creates a descriptor set layout create info.
	*/
	void CreateDescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo, const DynamicArray<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings) const CATALYST_NOEXCEPT;

};