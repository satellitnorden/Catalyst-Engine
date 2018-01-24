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
	VulkanSurface() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanSurface() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan surface.
	*/
	const VULKAN_SURFACE_TYPE& Get() const CATALYST_NOEXCEPT { return vulkanSurface; }

	/*
	*	Initializes this Vulkan surface.
	*/
	void Initialize(Window &window) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan surface.
	*/
	void Release() CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan surface.
	VULKAN_SURFACE_TYPE vulkanSurface;

};