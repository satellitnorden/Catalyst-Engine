#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanEvent final
{

public:

	/*
	*	Returns the underlying Vulkan event.
	*/
	const VkEvent& Get() const NOEXCEPT { return _VulkanEvent; }

	/*
	*	Initializes this Vulkan event.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Releases this Vulkan event.
	*/
	void Release() NOEXCEPT;

	/*
	*	Resets this Vulkan event.
	*/
	void Reset() NOEXCEPT;

	/*
	*	Sets this Vulkan event.
	*/
	void Set() NOEXCEPT;

private:

	//The underlying Vulkan event.
	VkEvent _VulkanEvent;

	/*
	*	Creates an event create info.
	*/
	void CreateEventCreateInfo(VkEventCreateInfo &eventCreateInfo) const NOEXCEPT;

};
#endif