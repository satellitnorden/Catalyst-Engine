#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanInstance final
{

public:

	/*
	*	Returns the underlying Vulkan instance.
	*/
	const VkInstance& Get() const NOEXCEPT { return _VulkanInstance; }

	/*
	*	Initializes this Vulkan instance.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Releases this Vulkan instance.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan instance.
	VkInstance _VulkanInstance;

	/*
	*	Creates the application info.
	*/
	void CreateApplicationInfo(VkApplicationInfo &applicationInfo) const NOEXCEPT;

	/*
	*	Creates the instance create info.
	*/
	void CreateInstanceCreateInfo(VkInstanceCreateInfo &createInstanceInfo, const VkApplicationInfo &applicationInfo) const NOEXCEPT;

};
#endif