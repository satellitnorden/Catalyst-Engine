#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanSurface final
{

public:

	/*
	*	Returns the underlying Vulkan surface.
	*/
	const VkSurfaceKHR& Get() const NOEXCEPT { return _VulkanSurface; }

	/*
	*	Initializes this Vulkan surface.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Releases this Vulkan surface.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan surface.
	VkSurfaceKHR _VulkanSurface;

};
#endif