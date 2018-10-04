//Header file.
#include <Rendering/API/Vulkan/VulkanDescriptorPool.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/API/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/API/Vulkan/VulkanInterface.h>
#include <Rendering/API/Vulkan/VulkanLogicalDevice.h>

//Vulkan descriptor pool constants.
namespace VulkanDescriptorPoolConstants
{
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_SETS{ 128 };
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_COMBINED_IMAGE_SAMPLERS{ 128 };
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_UNIFORM_BUFFERS{ 64 };
}

/*
*	Initializes this Vulkan descriptor pool.
*/
void VulkanDescriptorPool::Initialize() NOEXCEPT
{
	//Create a descriptor pool sizes.
	DynamicArray<VkDescriptorPoolSize> descriptorPoolSizes;
	CreateDescriptorPoolSizes(descriptorPoolSizes);

	//Create the descriptor pool create info.
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
	CreateDescriptorPoolCreateInfo(descriptorPoolCreateInfo, descriptorPoolSizes);

	//Create the Vulkan descriptor pool!
	VULKAN_ERROR_CHECK(vkCreateDescriptorPool(VulkanInterface::Instance->GetLogicalDevice().Get(), &descriptorPoolCreateInfo, nullptr, &_VulkanDescriptorPool));
}

/*
*	Releases this Vulkan descriptor pool.
*/
void VulkanDescriptorPool::Release() NOEXCEPT
{
	//Destroy this Vulkan descriptor pool.
	vkDestroyDescriptorPool(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDescriptorPool, nullptr);
}

/*
*	Allocates a Vulkan descriptor set.
*/
void VulkanDescriptorPool::AllocateDescriptorSet(VulkanDescriptorSet &vulkaDescriptorSet, const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) const NOEXCEPT
{
	//Lock the descriptor pool.
	ScopedLock<Spinlock> scopedLock{ _Lock };

	//Initialize the Vulkan descriptor set.
	vulkaDescriptorSet.Initialize(*this, vulkanDescriptorSetLayout);
}

/*
*	Frees a Vulkan descritor set.
*/
void VulkanDescriptorPool::FreeDescriptorSet(VkDescriptorSet descriptorSet) const NOEXCEPT
{
	//Lock the descriptor pool.
	ScopedLock<Spinlock> scopedLock{ _Lock };

	//Free the Vulkan descriptor set.
	vkFreeDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDescriptorPool, 1, &descriptorSet);
}

/*
*	Creates a descriptor pool size.
*/
void VulkanDescriptorPool::CreateDescriptorPoolSizes(DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const NOEXCEPT
{
	descriptorPoolSizes.Reserve(2);

	VkDescriptorPoolSize combinedImageSamplerDescriptorPoolSize;

	combinedImageSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	combinedImageSamplerDescriptorPoolSize.descriptorCount = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_UNIFORM_BUFFERS;

	descriptorPoolSizes.EmplaceFast(combinedImageSamplerDescriptorPoolSize);

	VkDescriptorPoolSize uniformBufferDescriptorPoolSize;

	uniformBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	uniformBufferDescriptorPoolSize.descriptorCount = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_COMBINED_IMAGE_SAMPLERS;

	descriptorPoolSizes.EmplaceFast(uniformBufferDescriptorPoolSize);
}

/*
*	Creates a descriptor pool create info.
*/
void VulkanDescriptorPool::CreateDescriptorPoolCreateInfo(VkDescriptorPoolCreateInfo &descriptorPoolCreateInfo, const DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const NOEXCEPT
{
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = nullptr;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_SETS;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(descriptorPoolSizes.Size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.Data();
}