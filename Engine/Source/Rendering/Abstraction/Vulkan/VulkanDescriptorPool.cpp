#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorPool.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>

//Vulkan descriptor pool constants.
namespace VulkanDescriptorPoolConstants
{
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_SETS{ 1'024 * 2 };
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_COMBINED_IMAGE_SAMPLERS{ 8'192 };
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_SAMPLED_IMAGES{ 8'192 };
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_SAMPLERS{ 32 };
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_UNIFORM_BUFFERS{ 256 };
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_STORAGE_BUFFERS{ 16 };
	constexpr uint32 VULKAN_DESCRIPTOR_POOL_MAXIMUM_STORAGE_IMAGES{ 32 };
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
	static uint32 number_of_descriptor_sets{ 0 };

	//Lock the descriptor pool.
	SCOPED_LOCK(_Lock);

	//Initialize the Vulkan descriptor set.
	vulkaDescriptorSet.Initialize(*this, vulkanDescriptorSetLayout);

	++number_of_descriptor_sets;
}

/*
*	Frees a Vulkan descritor set.
*/
void VulkanDescriptorPool::FreeDescriptorSet(VkDescriptorSet descriptorSet) const NOEXCEPT
{
	//Lock the descriptor pool.
	SCOPED_LOCK(_Lock);

	//Free the Vulkan descriptor set.
	vkFreeDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDescriptorPool, 1, &descriptorSet);
}

/*
*	Creates a descriptor pool size.
*/
void VulkanDescriptorPool::CreateDescriptorPoolSizes(DynamicArray<VkDescriptorPoolSize> &descriptorPoolSizes) const NOEXCEPT
{
	descriptorPoolSizes.Reserve(4);

	VkDescriptorPoolSize combinedImageSamplerDescriptorPoolSize;

	combinedImageSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	combinedImageSamplerDescriptorPoolSize.descriptorCount = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_COMBINED_IMAGE_SAMPLERS;

	descriptorPoolSizes.Emplace(combinedImageSamplerDescriptorPoolSize);

	VkDescriptorPoolSize sampledImageDescriptorPoolSize;

	sampledImageDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	sampledImageDescriptorPoolSize.descriptorCount = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_SAMPLED_IMAGES;

	descriptorPoolSizes.Emplace(sampledImageDescriptorPoolSize);

	VkDescriptorPoolSize samplersDescriptorPoolSize;

	samplersDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
	samplersDescriptorPoolSize.descriptorCount = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_SAMPLERS;

	descriptorPoolSizes.Emplace(samplersDescriptorPoolSize);

	VkDescriptorPoolSize uniformBufferDescriptorPoolSize;

	uniformBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferDescriptorPoolSize.descriptorCount = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_UNIFORM_BUFFERS;

	descriptorPoolSizes.Emplace(uniformBufferDescriptorPoolSize);

	VkDescriptorPoolSize storageBufferDescriptorPoolSize;

	storageBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	storageBufferDescriptorPoolSize.descriptorCount = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_STORAGE_BUFFERS;

	descriptorPoolSizes.Emplace(storageBufferDescriptorPoolSize);

	VkDescriptorPoolSize storageImageDescriptorPoolSize;

	storageImageDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	storageImageDescriptorPoolSize.descriptorCount = VulkanDescriptorPoolConstants::VULKAN_DESCRIPTOR_POOL_MAXIMUM_STORAGE_IMAGES;

	descriptorPoolSizes.Emplace(storageImageDescriptorPoolSize);
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
#endif