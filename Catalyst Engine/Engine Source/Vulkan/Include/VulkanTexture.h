#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;

class VulkanTexture
{

public:

	/*
	*	Default constructor.
	*/
	VulkanTexture() CATALYST_NOEXCEPT;

	/*
	*	Default desctuctor.
	*/
	~VulkanTexture() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan image.
	*/
	CATALYST_FORCE_INLINE const VkImage& Get() const CATALYST_NOEXCEPT { return vulkanImage; }

	/*
	*	Initializes this Vulkan texture.
	*/
	void Initialize(const uint32 width, const uint32 height, const byte *CATALYST_RESTRICT textureData) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan texture.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the Vulkan image view.
	*/
	CATALYST_FORCE_INLINE const VkImageView& GetImageView() const CATALYST_NOEXCEPT { return vulkanImageView; }

	/*
	*	Returns the Vulkan sampler.
	*/
	CATALYST_FORCE_INLINE const VkSampler& GetSampler() const CATALYST_NOEXCEPT { return vulkanSampler; }

	/*
	*	Returns the write descriptor set for this texture.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan image.
	VkImage vulkanImage{ nullptr };

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory{ nullptr };

	//The Vulkan image view.
	VkImageView vulkanImageView{ nullptr };

	//The Vulkan sampler.
	VkSampler vulkanSampler{ nullptr };

	//The descriptor image info for this texture.
	VkDescriptorImageInfo vulkanDescriptorImageInfo{ };

	//The write descriptor set for this texture.
	VkWriteDescriptorSet vulkanWriteDescriptorSet{ };

	/*
	*	Creates the descriptor image info.
	*/
	void CreateDescriptorImageInfo() CATALYST_NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() CATALYST_NOEXCEPT;

};