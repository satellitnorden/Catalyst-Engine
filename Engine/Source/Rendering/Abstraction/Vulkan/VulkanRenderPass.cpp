#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanRenderPass.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCommandPool.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanSemaphore.h>
#include <Rendering/Abstraction/Vulkan/VulkanSwapChain.h>
#include <Rendering/Abstraction/Vulkan/VulkanUniformBuffer.h>

/*
*	Initializes this Vulkan render pass.
*/
void VulkanRenderPass::Initialize(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT
{
	//Create the render pass create info.
	VkRenderPassCreateInfo renderPassCreateInfo;
	CreateRenderPassCreateInfo(renderPassCreateInfo, parameters);

	//Create the render pass!
	VULKAN_ERROR_CHECK(vkCreateRenderPass(VulkanInterface::Instance->GetLogicalDevice().Get(), &renderPassCreateInfo, nullptr, &_VulkanRenderPass));
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
#endif