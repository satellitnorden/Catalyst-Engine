#if defined(CATALYST_HAS_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanFramebuffer.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan framebuffer.
*/
void VulkanFramebuffer::Initialize(const VulkanFramebufferCreationParameters &parameters) NOEXCEPT
{
	//Set the parameters.
	_Parameters = parameters;

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
	framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(parameters._Attachments.Size());
	framebufferCreateInfo.pAttachments = parameters._Attachments.Data();
	framebufferCreateInfo.width = parameters._Extent.width;
	framebufferCreateInfo.height = parameters._Extent.height;
	framebufferCreateInfo.layers = parameters._NumberOfLayers;
}
#endif