#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanFramebuffer.h>

//Forward declarations.
class VulkanRenderPassCreationParameters;

class VulkanRenderPass final
{

public:

	/*
	*	Returns the underlying Vulkan render pass.
	*/
	const VkRenderPass& Get() const NOEXCEPT { return _VulkanRenderPass; }

	/*
	*	Initializes this Vulkan render pass.
	*/
	void Initialize(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT;

	/*
	*	Releases this Vulkan render pass.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan render pass.
	VkRenderPass _VulkanRenderPass;

	/*
	*	Creates a render pass create info.
	*/
	void CreateRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassCreateInfo, const VulkanRenderPassCreationParameters &parameters) const NOEXCEPT;

};
#endif