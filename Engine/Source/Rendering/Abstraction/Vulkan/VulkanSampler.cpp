#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanSampler.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan sampler.
*/
void VulkanSampler::Initialize(const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	//Create the sampler create info.
	VkSamplerCreateInfo samplerCreateInfo;
	CreateSamplerCreateInfo(samplerCreateInfo, magnificationFilter, mipmapMode, addressMode);

	//Create the sampler!
	VULKAN_ERROR_CHECK(vkCreateSampler(VulkanInterface::Instance->GetLogicalDevice().Get(), &samplerCreateInfo, nullptr, &_VulkanSampler));
}

/*
*	Releases this Vulkan sampler.
*/
void VulkanSampler::Release() NOEXCEPT
{
	//Destroy Vulkan sampler.
	vkDestroySampler(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanSampler, nullptr);
}

/*
*	Creates a sampler create info.
*/
void VulkanSampler::CreateSamplerCreateInfo(VkSamplerCreateInfo &samplerCreateInfo, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) const NOEXCEPT
{
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = nullptr;
	samplerCreateInfo.flags = 0;
	samplerCreateInfo.magFilter = magnificationFilter;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = mipmapMode;
	samplerCreateInfo.addressModeU = addressMode;
	samplerCreateInfo.addressModeV = addressMode;
	samplerCreateInfo.addressModeW = addressMode;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = FLOAT_MAXIMUM;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
}
#endif