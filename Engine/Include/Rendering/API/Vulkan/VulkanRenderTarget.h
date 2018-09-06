#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;

class VulkanRenderTarget final
{

public:

	/*
	*	Initializes this Vulkan render target.
	*/
	void Initialize(const VkExtent2D extent, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerAddressMode addressMode) NOEXCEPT;

	/*
	*	Releases this Vulkan render target.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the Vulkan image.
	*/
	const VkImage GetImage() const NOEXCEPT { return _VulkanImage; }

	/*
	*	Returns the Vulkan image view.
	*/
	const VkImageView GetImageView() const NOEXCEPT { return _VulkanImageView; }

	/*
	*	Returns the Vulkan sampler.
	*/
	const VkSampler GetSampler() const NOEXCEPT { return _VulkanSampler; }

	/*
	*	Returns the write descriptor set for this texture.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT;

	/*
	*	Returns the extent of the render target.
	*/
	VkExtent2D GetExtent() const NOEXCEPT { return _Extent; }

	/*
	*	Returns the format of the render target.
	*/
	VkFormat GetFormat() const NOEXCEPT { return _Format; }

private:

	//The Vulkan image.
	VkImage _VulkanImage;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The Vulkan image view.
	VkImageView _VulkanImageView;

	//The Vulkan sampler.
	VkSampler _VulkanSampler;

	//The descriptor image info for this texture.
	VkDescriptorImageInfo _VulkanDescriptorImageInfo;

	//The write descriptor set for this texture.
	VkWriteDescriptorSet _VulkanWriteDescriptorSet;

	//The extent of the image.
	VkExtent2D _Extent;

	//The format of the image.
	VkFormat _Format;

	/*
	*	Creates the descriptor image info.
	*/
	void CreateDescriptorImageInfo() NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() NOEXCEPT;

};