#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanEvent final
{

public:

	/*
	*	Returns the underlying Vulkan event.
	*/
	const VkEvent& Get() const NOEXCEPT { return vulkanEvent; }

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
	VkEvent vulkanEvent;

	/*
	*	Creates an event create info.
	*/
	void CreateEventCreateInfo(VkEventCreateInfo &eventCreateInfo) const NOEXCEPT;

};