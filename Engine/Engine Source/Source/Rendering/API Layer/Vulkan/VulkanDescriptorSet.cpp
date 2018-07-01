//Header file.
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSet.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanDescriptorPool.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan descriptor set.
*/
void VulkanDescriptorSet::Initialize(const VulkanDescriptorPool &vulkanDescriptorPool, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) NOEXCEPT
{
	//Create the descriptor set allocate info.
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
	CreateDescriptorSetAllocateInfo(descriptorSetAllocateInfo, vulkanDescriptorPool, vulkanDescriptorSetLayout);

	//Allocate the descriptor set!
	VULKAN_ERROR_CHECK(vkAllocateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), &descriptorSetAllocateInfo, &vulkanDescriptorSet));
}

/*
*	Creates a descriptor set allocate info.
*/
void VulkanDescriptorSet::CreateDescriptorSetAllocateInfo(VkDescriptorSetAllocateInfo &descriptorSetAllocateInfo, const VulkanDescriptorPool &vulkanDescriptorPool, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) const NOEXCEPT
{
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = vulkanDescriptorPool.Get();
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &vulkanDescriptorSetLayout.Get();
}