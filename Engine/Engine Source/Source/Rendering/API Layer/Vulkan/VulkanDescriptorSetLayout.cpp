//Header file.
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSetLayout.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanDescriptorSetLayout::VulkanDescriptorSetLayout() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() NOEXCEPT
{

}

/*
*	Initializes this Vulkan descriptor set layout.
*/
void VulkanDescriptorSetLayout::Initialize(const uint32 descriptorSetLayoutBindingCount, const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings) NOEXCEPT
{
	//Create the descriptor set layout create info.
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	CreateDescriptorSetLayoutCreateInfo(descriptorSetLayoutCreateInfo, descriptorSetLayoutBindingCount, descriptorSetLayoutBindings);

	//Create the descriptor set layout!
	VULKAN_ERROR_CHECK(vkCreateDescriptorSetLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), &descriptorSetLayoutCreateInfo, nullptr, &vulkanDescriptorSetLayout));
}

/*
*	Releases this Vulkan descriptor set layout.
*/
void VulkanDescriptorSetLayout::Release() NOEXCEPT
{
	//Destroy the descriptor set layout.
	vkDestroyDescriptorSetLayout(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDescriptorSetLayout, nullptr);
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