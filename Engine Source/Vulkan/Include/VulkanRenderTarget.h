#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

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
	*	Prepares this render target for reading.
	*/
	void PrepareForRead() CATALYST_NOEXCEPT;

	/*
	*	Prepares this render target for writing.
	*/
	void PrepareForWrite() CATALYST_NOEXCEPT;

private:

	//The Vulkan image.
	VkImage vulkanImage;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

	//The Vulkan image view.
	VkImageView vulkanImageView;

	//The Vulkan sampler.
	VkSampler vulkanSampler;

	//The current image layout.
	VkImageLayout currentImageLayout{ VK_IMAGE_LAYOUT_UNDEFINED };

};