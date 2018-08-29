#pragma once

//Core.
#include <Core/General/EngineCore.h>

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
	const VkRenderPass& Get() const NOEXCEPT { return vulkanRenderPass; }

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
	VkRenderPass vulkanRenderPass;

	/*
	*	Creates a render pass create info.
	*/
	void CreateRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassCreateInfo, const VulkanRenderPassCreationParameters &parameters) const NOEXCEPT;

};