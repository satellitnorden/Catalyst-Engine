#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

class VulkanIndexBuffer
{

public:

	/*
	*	Default constructor.
	*/
	VulkanIndexBuffer() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanIndexBuffer() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	CATALYST_FORCE_INLINE const VkBuffer& Get() const CATALYST_NOEXCEPT { return vulkanBuffer; }

	/*
	*	Initializes this Vulkan index buffer.
	*/
	void Initialize(const DynamicArray<uint32> &indices) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan index buffer.
	*/
	void Release() CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer vulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

};