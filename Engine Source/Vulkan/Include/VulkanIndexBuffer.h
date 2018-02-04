#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

class VulkanIndexBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanIndexBuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanIndexBuffer() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return vulkanBuffer; }

	/*
	*	Initializes this Vulkan index buffer.
	*/
	void Initialize(const DynamicArray<uint32> &indices) NOEXCEPT;

	/*
	*	Releases this Vulkan index buffer.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer vulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

};