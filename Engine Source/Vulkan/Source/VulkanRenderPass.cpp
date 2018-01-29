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
	DynamicArray<VkAttachmentReference> colorAttachmentReferences;
	CreateColorAttachmentReference(colorAttachmentReferences, vulkanPipelineCreationParameters);

	//Create the subpass description.
	VkSubpassDescription subpassDescription;
	CreateSubpassDescription(subpassDescription, depthAttachmentReference, colorAttachmentReferences, vulkanPipelineCreationParameters);

	//Create the subpass dependency.
	VkSubpassDependency subpassDependency;
	CreateSubpassDependency(subpassDependency);

	//Create the render pass create info.
	VkRenderPassCreateInfo renderPassCreateInfo;
	CreateRenderPassCreateInfo(renderPassCreateInfo, attachmentDescriptions, subpassDescription, subpassDependency);

	//Create the render pass!
	VULKAN_ERROR_CHECK(vkCreateRenderPass(VulkanInterface::Instance->GetLogicalDevice().Get(), &renderPassCreateInfo, nullptr, &vulkanRenderPass));

	//Create the framebuffers.
	framebuffers.Resize(vulkanPipelineCreationParameters.colorAttachments.Size());

	for (size_t i = 0, size = vulkanPipelineCreationParameters.colorAttachments.Size(); i < size; ++i)
	{
		framebuffers[i].Initialize(*this, vulkanPipelineCreationParameters.depthBuffers.Empty() ? nullptr : vulkanPipelineCreationParameters.depthBuffers[i], vulkanPipelineCreationParameters.colorAttachments[i], vulkanPipelineCreationParameters.viewportExtent);
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
	vkDestroyRenderPass(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanRenderPass, nullptr);
}

/*
*	Creates an attachment description.
*/
void VulkanRenderPass::CreateAttachmentDescriptions(DynamicArray<VkAttachmentDescription> &attachmentDescriptions, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const CATALYST_NOEXCEPT
{
	if (!vulkanPipelineCreationParameters.depthBuffers.Empty())
	{
		VkAttachmentDescription depthAttachmentDescription;

		depthAttachmentDescription.flags = 0;
		depthAttachmentDescription.format = VulkanInterface::Instance->GetSwapchain().GetDepthBuffer().GetFormat();
		depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentDescription.loadOp = vulkanPipelineCreationParameters.attachmentLoadOperator;
		depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription.initialLayout = vulkanPipelineCreationParameters.depthAttachmentInitialLayout;
		depthAttachmentDescription.finalLayout = vulkanPipelineCreationParameters.depthAttachmentFinalLayout;

		attachmentDescriptions.Emplace(depthAttachmentDescription);
	}

	for (VkImageView imageView : vulkanPipelineCreationParameters.colorAttachments[0])
	{
		VkAttachmentDescription colorAttachmentDescription;

		colorAttachmentDescription.flags = 0;
		colorAttachmentDescription.format = vulkanPipelineCreationParameters.colorAttachmentFormat;
		colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDescription.loadOp = vulkanPipelineCreationParameters.attachmentLoadOperator;
		colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDescription.initialLayout = vulkanPipelineCreationParameters.colorAttachmentInitialLayout;
		colorAttachmentDescription.finalLayout = vulkanPipelineCreationParameters.colorAttachmentFinalLayout;

		attachmentDescriptions.Emplace(colorAttachmentDescription);
	}
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
void VulkanRenderPass::CreateColorAttachmentReference(DynamicArray<VkAttachmentReference> &attachmentReferences, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const CATALYST_NOEXCEPT
{
	uint32 counter{ vulkanPipelineCreationParameters.depthBuffers.Empty() ? static_cast<uint32>(0) : static_cast<uint32>(1) };
	for (VkImageView colorAttachment : vulkanPipelineCreationParameters.colorAttachments[0])
	{
		VkAttachmentReference newAttachmentReference;
		newAttachmentReference.attachment = counter++;
		newAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachmentReferences.Emplace(newAttachmentReference);
	}
}

/*
*	Creates an subpass description.
*/
void VulkanRenderPass::CreateSubpassDescription(VkSubpassDescription &subpassDescription, const VkAttachmentReference &depthAttachmentReference, const DynamicArray<VkAttachmentReference> &colorAttachmentReferences, const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) const CATALYST_NOEXCEPT
{
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = static_cast<uint32>(colorAttachmentReferences.Size());
	subpassDescription.pColorAttachments = colorAttachmentReferences.Data();
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.pDepthStencilAttachment = vulkanPipelineCreationParameters.depthBuffers.Empty() ? nullptr : &depthAttachmentReference;
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