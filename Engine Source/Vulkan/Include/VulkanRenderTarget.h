#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;

class VulkanRenderTarget final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanRenderTarget() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanRenderTarget() CATALYST_NOEXCEPT;

	/*
	*	Initializes this Vulkan render target.
	*/
	void Initialize(const VkExtent2D extent) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan render target.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the Vulkan image.
	*/
	const VkImage& GetImage() const CATALYST_NOEXCEPT { return vulkanImage; }

	/*
	*	Returns the Vulkan image view.
	*/
	const VkImageView& GetImageView() const CATALYST_NOEXCEPT { return vulkanImageView; }

	/*
	*	Returns the Vulkan sampler.
	*/
	const VkSampler& GetSampler() const CATALYST_NOEXCEPT { return vulkanSampler; }

	/*
	*	Returns the write descriptor set for this texture.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const CATALYST_NOEXCEPT;

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

	/*
	*	Creates the descriptor image info.
	*/
	void CreateDescriptorImageInfo() CATALYST_NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() CATALYST_NOEXCEPT;

};