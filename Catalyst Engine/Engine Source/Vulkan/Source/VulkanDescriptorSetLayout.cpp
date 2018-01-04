//Header file.
#include <VulkanDescriptorSetLayout.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanDescriptorSetLayout::VulkanDescriptorSetLayout() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan descriptor set layout.
*/
void VulkanDescriptorSetLayout::Initialize(const DynamicArray<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings) CATALYST_NOEXCEPT
{
	//Create the descriptor set layout create info.
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	CreateDescriptorSetLayoutCreateInfo(descriptorSetLayoutCreateInfo, descriptorSetLayoutBindings);

	//Create the descriptor set layout!
	VkResult result = vkCreateDescriptorSetLayout(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &descriptorSetLayoutCreateInfo, nullptr, &vulkanDescriptorSetLayout);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif
}

/*
*	Releases this Vulkan descriptor set layout.
*/
void VulkanDescriptorSetLayout::Release() CATALYST_NOEXCEPT
{
	//Destroy the descriptor set layout.
	vkDestroyDescriptorSetLayout(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanDescriptorSetLayout, nullptr);
}

/*
*	Creates a descriptor set layout create info.
*/
void VulkanDescriptorSetLayout::CreateDescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo, const DynamicArray<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings) const CATALYST_NOEXCEPT
{
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(descriptorSetLayoutBindings.Size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.Data();
}