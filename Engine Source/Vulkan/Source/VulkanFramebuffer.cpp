//Header file.
#include <VulkanFramebuffer.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanFramebuffer::VulkanFramebuffer() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanFramebuffer::~VulkanFramebuffer() NOEXCEPT
{

}

/*
*	Initializes this Vulkan framebuffer.
*/
void VulkanFramebuffer::Initialize(const VulkanRenderPass &vulkanRenderPass, const VulkanDepthBuffer *RESTRICT depthBuffer, const DynamicArray<VkImageView> &colorAttachments, const VkExtent2D &extent) NOEXCEPT
{
	//Create the framebuffer create info.
	VkFramebufferCreateInfo framebufferCreateInfo;

	DynamicArray<VkImageView> attachments;

	if (depthBuffer)
	{
		attachments.Reserve(colorAttachments.Size() + 1);

		attachments.EmplaceUnsafe(depthBuffer->GetImageView());

		for (auto colorAttachment : colorAttachments)
		{
			attachments.EmplaceUnsafe(colorAttachment);
		}

		CreateFramebufferCreateInfo(framebufferCreateInfo, vulkanRenderPass, attachments, extent);
	}

	else
	{
		CreateFramebufferCreateInfo(framebufferCreateInfo, vulkanRenderPass, colorAttachments, extent);
	}

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
void VulkanFramebuffer::CreateFramebufferCreateInfo(VkFramebufferCreateInfo &framebufferCreateInfo, const VulkanRenderPass &vulkanRenderPass, const DynamicArray<VkImageView> &attachments, const VkExtent2D &extent) const NOEXCEPT
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