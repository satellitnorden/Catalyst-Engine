#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>
#include <VulkanFramebuffer.h>

//Forward declarations.
class VulkanPipelineCreationParameters;

class VulkanRenderPass final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanRenderPass() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanRenderPass() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan render pass.
	*/
	const VkRenderPass& Get() const CATALYST_NOEXCEPT { return vulkanRenderPass; }

	/*
	*	Initializes this Vulkan render pass.
	*/
	void Initialize(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan render pass.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the framebuffers for this Vulkan render pass.
	*/
	const DynamicArray<VulkanFramebuffer>& GetFrameBuffers() const CATALYST_NOEXCEPT { return framebuffers; }

private:

	//The underlying Vulkan render pass.
	VkRenderPass vulkanRenderPass;

	//Container for the Vulkan framebuffers for this Vulkan render pass.
	DynamicArray<VulkanFramebuffer> framebuffers;

	/*
	*	Creates an attachment description.
	*/
	void CreateAttachmentDescriptions(DynamicArray<VkAttachmentDescription> &attachmentDescriptions, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const CATALYST_NOEXCEPT;

	/*
	*	Creates a depth attachment reference.
	*/
	void CreateDepthAttachmentReference(VkAttachmentReference &attachmentReference) const CATALYST_NOEXCEPT;

	/*
	*	Creates a color attachment reference.
	*/
	void CreateColorAttachmentReference(VkAttachmentReference &attachmentReference) const CATALYST_NOEXCEPT;

	/*
	*	Creates an subpass description.
	*/
	void CreateSubpassDescription(VkSubpassDescription &subpassDescription, const VkAttachmentReference &depthAttachmentReference, const VkAttachmentReference &colorAttachmentReference) const CATALYST_NOEXCEPT;

	/*
	*	Creates an subpass dependency.
	*/
	void CreateSubpassDependency(VkSubpassDependency &subpassDependency) const CATALYST_NOEXCEPT;

	/*
	*	Creates a render pass create info.
	*/
	void CreateRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassCreateInfo, const DynamicArray<VkAttachmentDescription> &attachmentDescriptions, const VkSubpassDescription &subpassDescription, const VkSubpassDependency &subpassDependency) const CATALYST_NOEXCEPT;

};