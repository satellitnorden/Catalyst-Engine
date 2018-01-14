#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanLogicalDevice;
class VulkanRenderPass;

class VulkanFramebuffer
{

public:

	/*
	*	Default constructor.
	*/
	VulkanFramebuffer() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanFramebuffer() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan framebuffer.
	*/
	const VkFramebuffer& Get() const CATALYST_NOEXCEPT { return vulkanFramebuffer; }

	/*
	*	Initializes this Vulkan framebuffer.
	*/
	void Initialize(const VulkanRenderPass &vulkanRenderPass, const DynamicArray<VkImageView> &attachments, const VkExtent2D &extent) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan framebuffer.
	*/
	void Release() CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan framebuffer.
	VkFramebuffer vulkanFramebuffer;

	/*
	*	Creates a framebuffer create info.
	*/
	void CreateFramebufferCreateInfo(VkFramebufferCreateInfo &framebufferCreateInfo, const VulkanRenderPass &vulkanRenderPass, const DynamicArray<VkImageView> &attachments, const VkExtent2D &extent) const CATALYST_NOEXCEPT;

};