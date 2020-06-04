#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanSampler final
{

public:

	/*
	*	Returns the underlying Vulkan sampler.
	*/
	const VkSampler& Get() const NOEXCEPT { return _VulkanSampler; }

	/*
	*	Initializes this Vulkan sampler.
	*/
	void Initialize(const VkFilter magnification_filter, const VkSamplerMipmapMode mipmap_mode, const VkSamplerAddressMode address_mode, const uint8 anisotropic_samples) NOEXCEPT;

	/*
	*	Releases this Vulkan sampler.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan sampler.
	VkSampler _VulkanSampler;

	/*
	*	Creates a sampler create info.
	*/
	void CreateSamplerCreateInfo(VkSamplerCreateInfo &sampler_create_info, const VkFilter magnification_filter, const VkSamplerMipmapMode mipmap_mode, const VkSamplerAddressMode address_mode, const uint8 anisotropic_samples) const NOEXCEPT;

};
#endif