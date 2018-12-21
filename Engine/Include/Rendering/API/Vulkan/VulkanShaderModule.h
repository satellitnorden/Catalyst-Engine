#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanLogicalDevice;

class VulkanShaderModule final
{

public:

	/*
	*	Returns the underlying Vulkan shader module.
	*/
	const VkShaderModule Get() const NOEXCEPT { return _VulkanShaderModule; }

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
	const VkShaderStageFlagBits GetStage() const NOEXCEPT { return _Stage; }

private:

	//The underlying Vulkan shader module.
	VkShaderModule _VulkanShaderModule;

	//The stage where this shader module will be used.
	VkShaderStageFlagBits _Stage;

	/*
	*	Creates a shader module create info
	*/
	void CreateShaderModuleCreateInfo(VkShaderModuleCreateInfo &shaderModuleCreateInfo, const void* const shaderData, const uint64 shaderDataSize) const NOEXCEPT;

};
#endif