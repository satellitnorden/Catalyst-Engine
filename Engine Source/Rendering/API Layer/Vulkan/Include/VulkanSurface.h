#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class Window;

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
	const VULKAN_SURFACE_TYPE& Get() const NOEXCEPT { return vulkanSurface; }

	/*
	*	Initializes this Vulkan surface.
	*/
	void Initialize(Window &window) NOEXCEPT;

	/*
	*	Releases this Vulkan surface.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan surface.
	VULKAN_SURFACE_TYPE vulkanSurface;

};