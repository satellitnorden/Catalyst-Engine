#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

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
	void Initialize(const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT;

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
	void CreateSamplerCreateInfo(VkSamplerCreateInfo &samplerCreateInfo, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) const NOEXCEPT;

};