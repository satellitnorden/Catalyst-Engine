#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanDepthBuffer;
class VulkanLogicalDevice;
class VulkanRenderPass;

class VulkanFramebuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanFramebuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanFramebuffer() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan framebuffer.
	*/
	const VkFramebuffer& Get() const NOEXCEPT { return vulkanFramebuffer; }

	/*
	*	Initializes this Vulkan framebuffer.
	*/
	void Initialize(const VulkanRenderPass &vulkanRenderPass, const VulkanDepthBuffer *RESTRICT depthBuffer, const DynamicArray<VkImageView> &colorAttachments, const VkExtent2D &extent) NOEXCEPT;

	/*
	*	Releases this Vulkan framebuffer.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan framebuffer.
	VkFramebuffer vulkanFramebuffer;

	/*
	*	Creates a framebuffer create info.
	*/
	void CreateFramebufferCreateInfo(VkFramebufferCreateInfo &framebufferCreateInfo, const VulkanRenderPass &vulkanRenderPass, const DynamicArray<VkImageView> &attachments, const VkExtent2D &extent) const NOEXCEPT;

};