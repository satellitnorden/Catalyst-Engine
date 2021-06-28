#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanRenderPass.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan render pass.
*/
void VulkanRenderPass::Initialize(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT
{
	//Create the render pass create info.
	VkRenderPassCreateInfo render_pass_create_info;
	CreateRenderPassCreateInfo(render_pass_create_info, parameters);

	//Enable multiview.
	VkRenderPassMultiviewCreateInfoKHR render_pass_multiview_create_info;

	if (parameters._MultiviewEnabled)
	{
		EnableMultiview(&render_pass_multiview_create_info, &render_pass_create_info);
	}

	//Create the render pass!
	VULKAN_ERROR_CHECK(vkCreateRenderPass(VulkanInterface::Instance->GetLogicalDevice().Get(), &render_pass_create_info, nullptr, &_VulkanRenderPass));
}

/*
*	Releases this Vulkan render pass.
*/
void VulkanRenderPass::Release() NOEXCEPT
{
	//Destroy the Vulkan render pass.
	vkDestroyRenderPass(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanRenderPass, nullptr);
}

/*
*	Creates a render pass create info.
*/
void VulkanRenderPass::CreateRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassCreateInfo, const VulkanRenderPassCreationParameters &parameters) const NOEXCEPT
{
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0;
	renderPassCreateInfo.attachmentCount = parameters._AttachmentCount;
	renderPassCreateInfo.pAttachments = parameters._AttachmentDescriptions;
	renderPassCreateInfo.subpassCount = parameters._SubpassDescriptionCount;
	renderPassCreateInfo.pSubpasses = parameters._SubpassDescriptions;
	renderPassCreateInfo.dependencyCount = parameters._SubpassDependencyCount;
	renderPassCreateInfo.pDependencies = parameters._SubpassDependencies;
}

/*
*	Enables multiview.
*/
void VulkanRenderPass::EnableMultiview(VkRenderPassMultiviewCreateInfoKHR *const RESTRICT render_pass_multiview_create_info, VkRenderPassCreateInfo *const RESTRICT render_pass_create_info) NOEXCEPT
{
	//Define constants.
	constexpr uint32 MASK{ 0b00000011 };

	render_pass_multiview_create_info->sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO_KHR;
	render_pass_multiview_create_info->pNext = nullptr;
	render_pass_multiview_create_info->subpassCount = 1;
	render_pass_multiview_create_info->pViewMasks = &MASK;
	render_pass_multiview_create_info->dependencyCount = 0;
	render_pass_multiview_create_info->pViewOffsets = nullptr;
	render_pass_multiview_create_info->correlationMaskCount = 1;
	render_pass_multiview_create_info->pCorrelationMasks = &MASK;

	render_pass_create_info->pNext = render_pass_multiview_create_info;
}
#endif