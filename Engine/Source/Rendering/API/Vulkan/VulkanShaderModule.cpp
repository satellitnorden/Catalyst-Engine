#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/API/Vulkan/VulkanShaderModule.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan shader module.
*/
void VulkanShaderModule::Initialize(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits newStage) NOEXCEPT
{
	//Create the shader module create info.
	VkShaderModuleCreateInfo shaderModuleCreateInfo;
	CreateShaderModuleCreateInfo(shaderModuleCreateInfo, shaderData, shaderDataSize);

	//Create the shader module!
	VULKAN_ERROR_CHECK(vkCreateShaderModule(VulkanInterface::Instance->GetLogicalDevice().Get(), &shaderModuleCreateInfo, nullptr, &_VulkanShaderModule));

	//Set the stage.
	_Stage = newStage;
}

/*
*	Releases this Vulkan shader module.
*/
void VulkanShaderModule::Release() NOEXCEPT
{
	//Destroy the shader module.
	vkDestroyShaderModule(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanShaderModule, nullptr);
}

/*
*	Creates a shader module create info
*/
void VulkanShaderModule::CreateShaderModuleCreateInfo(VkShaderModuleCreateInfo &shaderModuleCreateInfo, const void* const shaderData, const uint64 shaderDataSize) const NOEXCEPT
{
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.pNext = nullptr;
	shaderModuleCreateInfo.flags = 0;
	shaderModuleCreateInfo.codeSize = shaderDataSize;
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32*>(shaderData);
}
#endif