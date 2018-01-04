//Header file.
#include <VulkanRenderPass.h>

//Graphics.
#include <Vertex.h>

//Vulkan.
#include <VulkanCommandPool.h>
#include <VulkanDescriptorSet.h>
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>
#include <VulkanPipeline.h>
#include <VulkanSemaphore.h>
#include <VulkanSwapChain.h>
#include <VulkanUniformBuffer.h>

/*
*	Default constructor.
*/
VulkanRenderPass::VulkanRenderPass() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanRenderPass::~VulkanRenderPass() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan render pass.
*/
void VulkanRenderPass::Initialize(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) CATALYST_NOEXCEPT
{
	//Create the attachment description.
	DynamicArray<VkAttachmentDescription> attachmentDescriptions;
	CreateAttachmentDescriptions(attachmentDescriptions, vulkanPipelineCreationParameters);

	//Create the depth attachment reference.
	VkAttachmentReference depthAttachmentReference;
	CreateDepthAttachmentReference(depthAttachmentReference);

	//Create the color attachment reference.
	VkAttachmentReference colorAttachmentReference;
	CreateColorAttachmentReference(colorAttachmentReference);

	//Create the subpass description.
	VkSubpassDescription subpassDescription;
	CreateSubpassDescription(subpassDescription, depthAttachmentReference, colorAttachmentReference);

	//Create the subpass dependency.
	VkSubpassDependency subpassDependency;
	CreateSubpassDependency(subpassDependency);

	//Create the render pass create info.
	VkRenderPassCreateInfo renderPassCreateInfo;
	CreateRenderPassCreateInfo(renderPassCreateInfo, attachmentDescriptions, subpassDescription, subpassDependency);

	//Create the render pass!
	VkResult result = vkCreateRenderPass(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &renderPassCreateInfo, nullptr, &vulkanRenderPass);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif

	//Create the framebuffers.
	framebuffers.Resize(vulkanPipelineCreationParameters.colorAttachmens.Size());

	for (size_t i = 0; i < vulkanPipelineCreationParameters.colorAttachmens.Size(); ++i)
	{
		DynamicArray<VkImageView> attachments;
		attachments.Reserve(vulkanPipelineCreationParameters.depthBuffer ? 2 : 1);

		if (vulkanPipelineCreationParameters.depthBuffer)
			attachments.EmplaceUnsafe(vulkanPipelineCreationParameters.depthBuffer->GetImageView());

		attachments.EmplaceUnsafe(vulkanPipelineCreationParameters.colorAttachmens[i]);

		framebuffers[i].Initialize(*this, attachments, vulkanPipelineCreationParameters.viewportExtent);
	}
}

/*
*	Releases this Vulkan render pass.
*/
void VulkanRenderPass::Release() CATALYST_NOEXCEPT
{
	//Destroy all framebuffers.
	for (auto &framebuffer : framebuffers)
	{
		framebuffer.Release();
	}

	//Destroy the Vulkan render pass.
	vkDestroyRenderPass(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanRenderPass, nullptr);
}

/*
*	Creates an attachment description.
*/
void VulkanRenderPass::CreateAttachmentDescriptions(DynamicArray<VkAttachmentDescription> &attachmentDescriptions, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const CATALYST_NOEXCEPT
{
	attachmentDescriptions.Reserve(2);

	VkAttachmentDescription depthAttachmentDescription;

	depthAttachmentDescription.flags = 0;
	depthAttachmentDescription.format = VulkanInterface::Instance->GetVulkanSwapchain().GetDepthBuffer().GetFormat();
	depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	attachmentDescriptions.EmplaceUnsafe(depthAttachmentDescription);

	VkAttachmentDescription colorAttachmentDescription;

	colorAttachmentDescription.flags = 0;
	colorAttachmentDescription.format = VulkanInterface::Instance->GetVulkanPhysicalDevice().GetSurfaceFormat().format;
	colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	attachmentDescriptions.EmplaceUnsafe(colorAttachmentDescription);
}

/*
*	Creates a depth attachment reference.
*/
void VulkanRenderPass::CreateDepthAttachmentReference(VkAttachmentReference &attachmentReference) const CATALYST_NOEXCEPT
{
	attachmentReference.attachment = 0;
	attachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

/*
*	Creates a color attachment reference.
*/
void VulkanRenderPass::CreateColorAttachmentReference(VkAttachmentReference &attachmentReference) const CATALYST_NOEXCEPT
{
	attachmentReference.attachment = 1;
	attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

/*
*	Creates an subpass description.
*/
void VulkanRenderPass::CreateSubpassDescription(VkSubpassDescription &subpassDescription, const VkAttachmentReference &depthAttachmentReference, const VkAttachmentReference &colorAttachmentReference) const CATALYST_NOEXCEPT
{
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;
}

/*
*	Creates an subpass dependency.
*/
void VulkanRenderPass::CreateSubpassDependency(VkSubpassDependency &subpassDependency) const CATALYST_NOEXCEPT
{
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags = 0;
}

/*
*	Creates a render pass create info.
*/
void VulkanRenderPass::CreateRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassCreateInfo, const DynamicArray<VkAttachmentDescription> &attachmentDescriptions, const VkSubpassDescription &subpassDescription, const VkSubpassDependency &subpassDependency) const CATALYST_NOEXCEPT
{
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0;
	renderPassCreateInfo.attachmentCount = static_cast<uint32>(attachmentDescriptions.Size());
	renderPassCreateInfo.pAttachments = attachmentDescriptions.Data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDescription;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &subpassDependency;
}