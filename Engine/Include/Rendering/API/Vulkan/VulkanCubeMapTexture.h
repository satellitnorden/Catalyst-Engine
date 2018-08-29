#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;

class VulkanCubeMapTexture final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanCubeMapTexture() NOEXCEPT;

	/*
	*	Default desctuctor.
	*/
	~VulkanCubeMapTexture() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan image.
	*/
	const VkImage& Get() const NOEXCEPT { return vulkanImage; }

	/*
	*	Initializes this texture.
	*/
	void Initialize(const float *const RESTRICT data, const uint32 width, const uint32 height) NOEXCEPT;

	/*
	*	Releases this texture.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the Vulkan image view.
	*/
	const VkImageView& GetImageView() const NOEXCEPT { return vulkanImageView; }

	/*
	*	Returns the Vulkan sampler.
	*/
	const VkSampler& GetSampler() const NOEXCEPT { return vulkanSampler; }

	/*
	*	Returns the write descriptor set for this texture.
	*/
	VkWriteDescriptorSet GetWriteDescriptorSet(const VulkanDescriptorSet &vulkanDescriptorSet, const uint32 binding) const NOEXCEPT;

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
	void CreateDescriptorImageInfo() NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() NOEXCEPT;

};