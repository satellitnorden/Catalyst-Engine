//Header file.
#include <VulkanFramebuffer.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanFramebuffer::VulkanFramebuffer() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanFramebuffer::~VulkanFramebuffer() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan framebuffer.
*/
void VulkanFramebuffer::Initialize(const VulkanRenderPass &vulkanRenderPass, const DynamicArray<VkImageView> &attachments, const VkExtent2D &extent) CATALYST_NOEXCEPT
{
	//Create the framebuffer create info.
	VkFramebufferCreateInfo framebufferCreateInfo;
	CreateFramebufferCreateInfo(framebufferCreateInfo, vulkanRenderPass, attachments, extent);

	//Create the Vulkan framebuffer!
	VkResult result = vkCreateFramebuffer(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &framebufferCreateInfo, nullptr, &vulkanFramebuffer);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif
}

/*
*	Releases this Vulkan framebuffer.
*/
void VulkanFramebuffer::Release() CATALYST_NOEXCEPT
{
	//Destroy the Vulkan framebuffer.
	vkDestroyFramebuffer(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanFramebuffer, nullptr);
}

/*
*	Creates a framebuffer create info.
*/
void VulkanFramebuffer::CreateFramebufferCreateInfo(VkFramebufferCreateInfo &framebufferCreateInfo, const VulkanRenderPass &vulkanRenderPass, const DynamicArray<VkImageView> &attachments, const VkExtent2D &extent) const CATALYST_NOEXCEPT
{
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = vulkanRenderPass.Get();
	framebufferCreateInfo.attachmentCount = static_cast<uint32>(attachments.Size());
	framebufferCreateInfo.pAttachments = attachments.Data();
	framebufferCreateInfo.width = extent.width;
	framebufferCreateInfo.height = extent.height;
	framebufferCreateInfo.layers = 1;
}