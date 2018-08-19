#pragma once

//Core.
#include <Core/EngineCore.h>

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
	const VkImage GetImage() const NOEXCEPT { return vulkanImage; }

	/*
	*	Returns the Vulkan image view.
	*/
	const VkImageView GetImageView() const NOEXCEPT { return vulkanImageView; }

	/*
	*	Returns the Vulkan sampler.
	*/
	const VkSampler GetSampler() const NOEXCEPT { return vulkanSampler; }

	/*
	*	Returns the write descriptor set for this texture.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT;

	/*
	*	Returns the extent of the render target.
	*/
	VkExtent2D GetExtent() const NOEXCEPT { return extent; }

	/*
	*	Returns the format of the render target.
	*/
	VkFormat GetFormat() const NOEXCEPT { return format; }

private:

	//The Vulkan image.
	VkImage vulkanImage;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

	//The Vulkan image view.
	VkImageView vulkanImageView;

	//The Vulkan sampler.
	VkSampler vulkanSampler;

	//The descriptor image info for this texture.
	VkDescriptorImageInfo vulkanDescriptorImageInfo;

	//The write descriptor set for this texture.
	VkWriteDescriptorSet vulkanWriteDescriptorSet;

	//The extent of the image.
	VkExtent2D extent;

	//The format of the image.
	VkFormat format;

	/*
	*	Creates the descriptor image info.
	*/
	void CreateDescriptorImageInfo() NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() NOEXCEPT;

};