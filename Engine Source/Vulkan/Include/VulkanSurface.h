#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanInstance;
class Window;

class VulkanSurface
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
	CATALYST_FORCE_INLINE const VULKAN_SURFACE_TYPE& Get() const CATALYST_NOEXCEPT { return vulkanSurface; }

	/*
	*	Initializes this Vulkan surface.
	*/
	void Initialize(const VulkanInstance &vulkanInstance, Window &window) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan surface.
	*/
	void Release(const VulkanInstance &vulkanInstance) CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan surface.
	VULKAN_SURFACE_TYPE vulkanSurface;

};