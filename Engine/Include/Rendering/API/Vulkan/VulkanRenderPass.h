#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanFramebuffer.h>

//Forward declarations.
class VulkanPipelineCreationParameters;
class VulkanRenderPassCreationParameters;

class VulkanRenderPass final
{

public:

	/*
	*	Returns the underlying Vulkan render pass.
	*/
	const VkRenderPass& Get() const NOEXCEPT { return vulkanRenderPass; }

	/*
	*	Initializes this Vulkan render pass.
	*/
	void Initialize(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) NOEXCEPT;

	/*
	*	Initializes this Vulkan render pass.
	*/
	void Initialize(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT;

	/*
	*	Releases this Vulkan render pass.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the framebuffers for this Vulkan render pass.
	*/
	const DynamicArray<VulkanFramebuffer>& GetFrameBuffers() const NOEXCEPT { return framebuffers; }

private:

	//The underlying Vulkan render pass.
	VkRenderPass vulkanRenderPass;

	//Container for the Vulkan framebuffers for this Vulkan render pass.
	DynamicArray<VulkanFramebuffer> framebuffers;

	/*
	*	Creates an attachment description.
	*/
	void CreateAttachmentDescriptions(DynamicArray<VkAttachmentDescription> &attachmentDescriptions, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates a depth attachment reference.
	*/
	void CreateDepthAttachmentReference(VkAttachmentReference &attachmentReference) const NOEXCEPT;

	/*
	*	Creates a color attachment reference.
	*/
	void CreateColorAttachmentReference(DynamicArray<VkAttachmentReference> &attachmentReferences, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates an subpass description.
	*/
	void CreateSubpassDescription(VkSubpassDescription &subpassDescription, const VkAttachmentReference &depthAttachmentReference, const DynamicArray<VkAttachmentReference> &colorAttachmentReferences, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const NOEXCEPT;

	/*
	*	Creates an subpass dependency.
	*/
	void CreateSubpassDependency(VkSubpassDependency &subpassDependency) const NOEXCEPT;

	/*
	*	Creates a render pass create info.
	*/
	void CreateRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassCreateInfo, const DynamicArray<VkAttachmentDescription> &attachmentDescriptions, const VkSubpassDescription &subpassDescription, const VkSubpassDependency &subpassDependency) const NOEXCEPT;

	/*
	*	Creates a render pass create info.
	*/
	void CreateRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassCreateInfo, const VulkanRenderPassCreationParameters &parameters) const NOEXCEPT;

};