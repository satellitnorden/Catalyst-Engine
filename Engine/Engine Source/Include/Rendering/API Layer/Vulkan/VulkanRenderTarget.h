#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanDescriptorSet;

class VulkanRenderTarget final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanRenderTarget() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanRenderTarget() NOEXCEPT;

	/*
	*	Initializes this Vulkan render target.
	*/
	void Initialize(const VkExtent2D extent) NOEXCEPT;

	/*
	*	Releases this Vulkan render target.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the Vulkan image.
	*/
	const VkImage& GetImage() const NOEXCEPT { return vulkanImage; }

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

	/*
	*	Returns the data contained in the image.
	*/
	void GetImageData(void *const RESTRICT imageData) NOEXCEPT;

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
	VkExtent2D imageExtent;

	/*
	*	Creates the descriptor image info.
	*/
	void CreateDescriptorImageInfo() NOEXCEPT;

	/*
	*	Creates the write descriptor set.
	*/
	void CreateWriteDescriptorSet() NOEXCEPT;

};