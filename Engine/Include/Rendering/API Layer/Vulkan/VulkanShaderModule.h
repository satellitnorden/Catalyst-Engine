#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanLogicalDevice;

class VulkanShaderModule final
{

public:

	/*
	*	Returns the underlying Vulkan shader module.
	*/
	const VkShaderModule Get() const NOEXCEPT { return vulkanShaderModule; }

	/*
	*	Initializes this Vulkan shader module.
	*/
	void Initialize(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits newStage) NOEXCEPT;

	/*
	*	Releases this Vulkan shader module.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the stage where this shader module will be used.
	*/
	const VkShaderStageFlagBits GetStage() const NOEXCEPT { return stage; }

private:

	//The underlying Vulkan shader module.
	VkShaderModule vulkanShaderModule;

	//The stage where this shader module will be used.
	VkShaderStageFlagBits stage;

	/*
	*	Creates a shader module create info
	*/
	void CreateShaderModuleCreateInfo(VkShaderModuleCreateInfo &shaderModuleCreateInfo, const void* const shaderData, const uint64 shaderDataSize) const NOEXCEPT;

};