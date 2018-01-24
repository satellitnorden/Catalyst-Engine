#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanLogicalDevice;

class VulkanShaderModule final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanShaderModule() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanShaderModule() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan shader module.
	*/
	const VkShaderModule Get() const CATALYST_NOEXCEPT { return vulkanShaderModule; }

	/*
	*	Initializes this Vulkan shader module.
	*/
	void Initialize(const DynamicArray<char> &shaderByteCode, const VkShaderStageFlagBits newStage) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan shader module.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the stage where this shader module will be used.
	*/
	const VkShaderStageFlagBits GetStage() const CATALYST_NOEXCEPT { return stage; }

private:

	//The underlying Vulkan shader module.
	VkShaderModule vulkanShaderModule;

	//The stage where this shader module will be used.
	VkShaderStageFlagBits stage;

	/*
	*	Creates a shader module create info
	*/
	void CreateShaderModuleCreateInfo(VkShaderModuleCreateInfo &shaderModuleCreateInfo, const DynamicArray<char> & shaderByteCode) const CATALYST_NOEXCEPT;

};