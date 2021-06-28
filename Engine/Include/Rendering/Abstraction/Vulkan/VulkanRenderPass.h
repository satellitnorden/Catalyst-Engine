#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanFramebuffer.h>

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

	/*
	*	Enables multiview.
	*/
	void EnableMultiview(VkRenderPassMultiviewCreateInfoKHR *const RESTRICT render_pass_multiview_create_info, VkRenderPassCreateInfo *const RESTRICT render_pass_create_info) NOEXCEPT;

};
#endif