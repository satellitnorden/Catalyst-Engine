//Header file.
#include <Rendering/API/Vulkan/VulkanFramebuffer.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan framebuffer.
*/
void VulkanFramebuffer::Initialize(const VulkanFramebufferCreationParameters &parameters) NOEXCEPT
{
	//Create the framebuffer create info.
	VkFramebufferCreateInfo framebufferCreateInfo;
	CreateFramebufferCreateInfo(framebufferCreateInfo, parameters);

	//Create the Vulkan framebuffer!
	VULKAN_ERROR_CHECK(vkCreateFramebuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), &framebufferCreateInfo, nullptr, &vulkanFramebuffer));
}

/*
*	Releases this Vulkan framebuffer.
*/
void VulkanFramebuffer::Release() NOEXCEPT
{
	//Destroy the Vulkan framebuffer.
	vkDestroyFramebuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanFramebuffer, nullptr);
}

/*
*	Creates a framebuffer create info.
*/
void VulkanFramebuffer::CreateFramebufferCreateInfo(VkFramebufferCreateInfo &framebufferCreateInfo, const VulkanFramebufferCreationParameters &parameters) const NOEXCEPT
{
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = parameters.renderPass;
	framebufferCreateInfo.attachmentCount = parameters.attachmentCount;
	framebufferCreateInfo.pAttachments = parameters.attachments;
	framebufferCreateInfo.width = parameters.extent.width;
	framebufferCreateInfo.height = parameters.extent.height;
	framebufferCreateInfo.layers = 1;
}