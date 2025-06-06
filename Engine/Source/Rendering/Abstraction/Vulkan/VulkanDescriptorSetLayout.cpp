#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSetLayout.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan descriptor set layout.
*/
void VulkanDescriptorSetLayout::Initialize(const uint32 descriptorSetLayoutBindingCount, const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings) NOEXCEPT
{
	//Create the descriptor set layout create info.
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	CreateDescriptorSetLayoutCreateInfo(descriptorSetLayoutCreateInfo, descriptorSetLayoutBindingCount, descriptorSetLayoutBindings);

	//Create the descriptor set layout!
	VULKAN_ERROR_CHECK(vkCreateDescriptorSetLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), &descriptorSetLayoutCreateInfo, nullptr, &_VulkanDescriptorSetLayout));
}

/*
*	Releases this Vulkan descriptor set layout.
*/
void VulkanDescriptorSetLayout::Release() NOEXCEPT
{
	//Destroy the descriptor set layout.
	vkDestroyDescriptorSetLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDescriptorSetLayout, nullptr);
}

/*
*	Creates a descriptor set layout create info.
*/
void VulkanDescriptorSetLayout::CreateDescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo, const uint32 descriptorSetLayoutBindingCount, const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings) const NOEXCEPT
{
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindingCount;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings;
}
#endif