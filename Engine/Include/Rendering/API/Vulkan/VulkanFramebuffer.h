#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanFramebufferCreationParameters.h>

class VulkanFramebuffer final
{

public:

	/*
	*	Returns the underlying Vulkan framebuffer.
	*/
	const VkFramebuffer& Get() const NOEXCEPT { return vulkanFramebuffer; }

	/*
	*	Initializes this Vulkan framebuffer.
	*/
	void Initialize(const VulkanFramebufferCreationParameters &parameters) NOEXCEPT;

	/*
	*	Releases this Vulkan framebuffer.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan framebuffer.
	VkFramebuffer vulkanFramebuffer;

	/*
	*	Creates a framebuffer create info.
	*/
	void CreateFramebufferCreateInfo(VkFramebufferCreateInfo &framebufferCreateInfo, const VulkanFramebufferCreationParameters &parameters) const NOEXCEPT;

};