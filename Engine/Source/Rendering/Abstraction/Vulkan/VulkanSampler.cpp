#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanSampler.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan sampler.
*/
void VulkanSampler::Initialize(const VkFilter magnification_filter, const VkSamplerMipmapMode mipmap_mode, const VkSamplerAddressMode address_mode, const uint8 anisotropic_samples) NOEXCEPT
{
	//Create the sampler create info.
	VkSamplerCreateInfo sampler_create_info;
	CreateSamplerCreateInfo(sampler_create_info, magnification_filter, mipmap_mode, address_mode, anisotropic_samples);

	//Create the sampler!
	VULKAN_ERROR_CHECK(vkCreateSampler(VulkanInterface::Instance->GetLogicalDevice().Get(), &sampler_create_info, nullptr, &_VulkanSampler));
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
void VulkanSampler::CreateSamplerCreateInfo(VkSamplerCreateInfo &sampler_create_info, const VkFilter magnification_filter, const VkSamplerMipmapMode mipmap_mode, const VkSamplerAddressMode address_mode, const uint8 anisotropic_samples) const NOEXCEPT
{
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.pNext = nullptr;
	sampler_create_info.flags = 0;
	sampler_create_info.magFilter = magnification_filter;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.mipmapMode = mipmap_mode;
	sampler_create_info.addressModeU = address_mode;
	sampler_create_info.addressModeV = address_mode;
	sampler_create_info.addressModeW = address_mode;
	sampler_create_info.mipLodBias = 0.0f;

	if (anisotropic_samples == 0 || VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceFeatures().samplerAnisotropy == VK_FALSE)
	{
		sampler_create_info.anisotropyEnable = VK_FALSE;
		sampler_create_info.maxAnisotropy = 0.0f;
	}

	else
	{
		sampler_create_info.anisotropyEnable = VK_TRUE;
		sampler_create_info.maxAnisotropy = static_cast<float32>(anisotropic_samples);
	}
	
	sampler_create_info.compareEnable = VK_FALSE;
	sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_create_info.minLod = 0.0f;
	sampler_create_info.maxLod = FLOAT_MAXIMUM;
	sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler_create_info.unnormalizedCoordinates = VK_FALSE;
}
#endif