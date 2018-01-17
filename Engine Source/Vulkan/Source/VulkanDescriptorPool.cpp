//Header file.
#include <VulkanDescriptorPool.h>

//Vulkan.
#include <VulkanDescriptorSet.h>
#include <VulkanDescriptorSetLayout.h>
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>

//Preprocessor defines.
#define VULKAN_DESCRIPTOR_POOL_MAXIMUM_SETS 4'096
#define VULKAN_DESCRIPTOR_POOL_MAXIMUM_UNIFORM_BUFFERS 8'192
#define VULKAN_DESCRIPTOR_POOL_MAXIMUM_COMBINED_IMAGE_SAMPLERS 32'768

/*
*	Default constructor.
*/
VulkanDescriptorPool::VulkanDescriptorPool() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanDescriptorPool::~VulkanDescriptorPool() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan descriptor pool.
*/
void VulkanDescriptorPool::Initialize() CATALYST_NOEXCEPT
{
	//Create a descriptor pool sizes.
	DynamicArray<VkDescriptorPoolSize> descriptorPoolSizes;
	CreateDescriptorPoolSizes(descriptorPoolSizes);

	//Create the descriptor pool create info.
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
	CreateDescriptorPoolCreateInfo(descriptorPoolCreateInfo, descriptorPoolSizes);

	//Create the Vulkan descriptor pool!
	VULKAN_ERROR_CHECK(vkCreateDescriptorPool(VulkanInterface::Instance->GetLogicalDevice().Get(), &descriptorPoolCreateInfo, nullptr, &vulkanDescriptorPool));
}

/*
*	Releases this Vulkan descriptor pool.
*/
void VulkanDescriptorPool::Release() CATALYST_NOEXCEPT
{
	//Destroy this Vulkan descriptor pool.
	vkDestroyDescriptorPool(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanDescriptorPool, nullptr);
}

/*
*	Allocates a Vulkan descriptor set.
*/
void VulkanDescriptorPool::AllocateDescriptorSet(VulkanDescriptorSet &vulkaDescriptorSet, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) const CATALYST_NOEXCEPT
{
	//Initialize the Vulkan descriptor set.
	vulkaDescriptorSet.Initialize(*this, vulkanDescriptorSetLayout);
}

/*
*	Creates a descriptor pool size.
*/
void VulkanDescriptorPool::CreateDescriptorPoolSizes(DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const CATALYST_NOEXCEPT
{
	descriptorPoolSizes.Reserve(2);

	VkDescriptorPoolSize combinedImageSamplerDescriptorPoolSize;

	combinedImageSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	combinedImageSamplerDescriptorPoolSize.descriptorCount = VULKAN_DESCRIPTOR_POOL_MAXIMUM_UNIFORM_BUFFERS;

	descriptorPoolSizes.EmplaceUnsafe(combinedImageSamplerDescriptorPoolSize);

	VkDescriptorPoolSize uniformBufferDescriptorPoolSize;

	uniformBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	uniformBufferDescriptorPoolSize.descriptorCount = VULKAN_DESCRIPTOR_POOL_MAXIMUM_COMBINED_IMAGE_SAMPLERS;

	descriptorPoolSizes.EmplaceUnsafe(uniformBufferDescriptorPoolSize);
}

/*
*	Creates a descriptor pool create info.
*/
void VulkanDescriptorPool::CreateDescriptorPoolCreateInfo(VkDescriptorPoolCreateInfo &descriptorPoolCreateInfo, const DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const CATALYST_NOEXCEPT
{
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = nullptr;
	descriptorPoolCreateInfo.flags = 0;
	descriptorPoolCreateInfo.maxSets = VULKAN_DESCRIPTOR_POOL_MAXIMUM_SETS;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(descriptorPoolSizes.Size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.Data();
}