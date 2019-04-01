#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanFramebuffer.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan framebuffer.
*/
void VulkanFramebuffer::Initialize(const VulkanFramebufferCreationParameters &parameters) NOEXCEPT
{
	//Create the framebuffer create info.
	VkFramebufferCreateInfo framebufferCreateInfo;
	CreateFramebufferCreateInfo(framebufferCreateInfo, parameters);

	//Create the Vulkan framebuffer!
	VULKAN_ERROR_CHECK(vkCreateFramebuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), &framebufferCreateInfo, nullptr, &_VulkanFramebuffer));
}

/*
*	Releases this Vulkan framebuffer.
*/
void VulkanFramebuffer::Release() NOEXCEPT
{
	//Destroy the Vulkan framebuffer.
	vkDestroyFramebuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanFramebuffer, nullptr);
}

/*
*	Creates a framebuffer create info.
*/
void VulkanFramebuffer::CreateFramebufferCreateInfo(VkFramebufferCreateInfo &framebufferCreateInfo, const VulkanFramebufferCreationParameters &parameters) const NOEXCEPT
{
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = parameters._RenderPass;
	framebufferCreateInfo.attachmentCount = parameters._AttachmentCount;
	framebufferCreateInfo.pAttachments = parameters._Attachments;
	framebufferCreateInfo.width = parameters._Extent.width;
	framebufferCreateInfo.height = parameters._Extent.height;
	framebufferCreateInfo.layers = 1;
}
#endif