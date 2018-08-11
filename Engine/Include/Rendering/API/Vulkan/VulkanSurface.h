#pragma once

//Core.
#include <Core/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

//Forward declarations.
class Surface;

class VulkanSurface final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanSurface() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanSurface() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan surface.
	*/
	const VkSurfaceKHR& Get() const NOEXCEPT { return vulkanSurface; }

	/*
	*	Initializes this Vulkan surface.
	*/
	void Initialize(Surface &surface) NOEXCEPT;

	/*
	*	Releases this Vulkan surface.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan surface.
	VkSurfaceKHR vulkanSurface;

};