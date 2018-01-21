#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;

class VulkanCubeMapTexture
{

public:

	/*
	*	Default constructor.
	*/
	VulkanCubeMapTexture() CATALYST_NOEXCEPT;

	/*
	*	Default desctuctor.
	*/
	~VulkanCubeMapTexture() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan image.
	*/
	const VkImage& Get() const CATALYST_NOEXCEPT { return vulkanImage; }

	/*
	*	Initializes this texture.
	*/
	void Initialize(const uint32 width, const uint32 height, const byte *CATALYST_RESTRICT *CATALYST_RESTRICT textureData) CATALYST_NOEXCEPT;

	/*
	*	Releases this texture.
	*/
	void Release() CATALYST_NOEXCEPT;

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

	//The underlying Vulkan image.
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