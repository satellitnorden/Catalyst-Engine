#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

class VulkanInstance
{

public:

	/*
	*	Default constructor.
	*/
	VulkanInstance() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanInstance() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan instance.
	*/
	CATALYST_FORCE_INLINE const VkInstance& Get() const CATALYST_NOEXCEPT { return vulkanInstance; }

	/*
	*	Initializes this Vulkan instance.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan instance.
	*/
	void Release() CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan instance.
	VkInstance vulkanInstance{ nullptr };

	/*
	*	Creates the application info.
	*/
	void CreateApplicationInfo(VkApplicationInfo &applicationInfo) const CATALYST_NOEXCEPT;

	/*
	*	Creates the instance create info.
	*/
	void CreateInstanceCreateInfo(VkInstanceCreateInfo &createInstanceInfo, const VkApplicationInfo &applicationInfo) const CATALYST_NOEXCEPT;

};