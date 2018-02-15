//Header file.
#include <VulkanDescriptorSetLayout.h>

//Vulkan.
#include <VulkanInterface.h>

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
void VulkanDescriptorSetLayout::Initialize(const DynamicArray<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings) NOEXCEPT
{
	//Create the descriptor set layout create info.
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	CreateDescriptorSetLayoutCreateInfo(descriptorSetLayoutCreateInfo, descriptorSetLayoutBindings);

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
void VulkanDescriptorSetLayout::CreateDescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo, const DynamicArray<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings) const NOEXCEPT
{
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(descriptorSetLayoutBindings.Size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.Data();
}