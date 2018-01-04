//Header file.
#include <VulkanShaderModule.h>

//Vulkan.
#include <VulkanInterface.h>

/*
*	Default constructor.
*/
VulkanShaderModule::VulkanShaderModule() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanShaderModule::~VulkanShaderModule() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan shader module.
*/
void VulkanShaderModule::Initialize(const DynamicArray<char> &shaderByteCode, const VkShaderStageFlagBits newStage) CATALYST_NOEXCEPT
{
	//Create the shader module create info.
	VkShaderModuleCreateInfo shaderModuleCreateInfo;
	CreateShaderModuleCreateInfo(shaderModuleCreateInfo, shaderByteCode);

	//Create the shader module!
	VkResult result = vkCreateShaderModule(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), &shaderModuleCreateInfo, nullptr, &vulkanShaderModule);

#if !defined(CATALYST_FINAL)
	if (result != VK_SUCCESS)
		BREAKPOINT;
#endif

	//Set the stage.
	stage = newStage;
}

/*
*	Releases this Vulkan shader module.
*/
void VulkanShaderModule::Release() CATALYST_NOEXCEPT
{
	//Destroy the shader module.
	vkDestroyShaderModule(VulkanInterface::Instance->GetVulkanLogicalDevice().Get(), vulkanShaderModule, nullptr);
}

/*
*	Creates a shader module create info
*/
void VulkanShaderModule::CreateShaderModuleCreateInfo(VkShaderModuleCreateInfo &shaderModuleCreateInfo, const DynamicArray<char> & shaderByteCode) const CATALYST_NOEXCEPT
{
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.pNext = nullptr;
	shaderModuleCreateInfo.flags = 0;
	shaderModuleCreateInfo.codeSize = shaderByteCode.Size();
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32*>(shaderByteCode.Data());
}