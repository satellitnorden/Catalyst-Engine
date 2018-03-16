#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

class VulkanInstance final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanInstance() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanInstance() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan instance.
	*/
	const VkInstance& Get() const NOEXCEPT { return vulkanInstance; }

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
	VkInstance vulkanInstance;

	/*
	*	Creates the application info.
	*/
	void CreateApplicationInfo(VkApplicationInfo &applicationInfo) const NOEXCEPT;

	/*
	*	Creates the instance create info.
	*/
	void CreateInstanceCreateInfo(VkInstanceCreateInfo &createInstanceInfo, const VkApplicationInfo &applicationInfo) const NOEXCEPT;

};