#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

class VulkanDescriptorSetLayout final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanDescriptorSetLayout() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanDescriptorSetLayout() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan descriptor set layout.
	*/
	const VkDescriptorSetLayout& Get() const NOEXCEPT { return vulkanDescriptorSetLayout; }

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
	VkDescriptorSetLayout vulkanDescriptorSetLayout;

	/*
	*	Creates a descriptor set layout create info.
	*/
	void CreateDescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo, const uint32 descriptorSetLayoutBindingCount, const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings) const NOEXCEPT;

};