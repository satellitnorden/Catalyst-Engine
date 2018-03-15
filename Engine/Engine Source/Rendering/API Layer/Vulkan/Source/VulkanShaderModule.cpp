//Header file.
#include <VulkanShaderModule.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanShaderModule::VulkanShaderModule() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanShaderModule::~VulkanShaderModule() NOEXCEPT
{

}

/*
*	Initializes this Vulkan shader module.
*/
void VulkanShaderModule::Initialize(const DynamicArray<char> &shaderByteCode, const VkShaderStageFlagBits newStage) NOEXCEPT
{
	//Create the shader module create info.
	VkShaderModuleCreateInfo shaderModuleCreateInfo;
	CreateShaderModuleCreateInfo(shaderModuleCreateInfo, shaderByteCode);

	//Create the shader module!
	VULKAN_ERROR_CHECK(vkCreateShaderModule(VulkanInterface::Instance->GetLogicalDevice().Get(), &shaderModuleCreateInfo, nullptr, &vulkanShaderModule));

	//Set the stage.
	stage = newStage;
}

/*
*	Releases this Vulkan shader module.
*/
void VulkanShaderModule::Release() NOEXCEPT
{
	//Destroy the shader module.
	vkDestroyShaderModule(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanShaderModule, nullptr);
}

/*
*	Creates a shader module create info
*/
void VulkanShaderModule::CreateShaderModuleCreateInfo(VkShaderModuleCreateInfo &shaderModuleCreateInfo, const DynamicArray<char> & shaderByteCode) const NOEXCEPT
{
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.pNext = nullptr;
	shaderModuleCreateInfo.flags = 0;
	shaderModuleCreateInfo.codeSize = shaderByteCode.Size();
	shaderModuleCreateInfo.pCode = ReinterpretCast<const uint32*>(shaderByteCode.Data());
}