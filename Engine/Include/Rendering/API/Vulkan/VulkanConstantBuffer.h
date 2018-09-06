#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>

class VulkanConstantBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanConstantBuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanConstantBuffer() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return _VulkanBuffer; }

	/*
	*	Initializes this Vulkan buffer.
	*
	*	data - Pointer to an array of pointers to the data that should be copied into the buffer.
	*	dataSizes - Pointer to an array of offsets for the data that should be copied into the buffer.
	*	dataChunks - The number of data chunks that should be copied into the buffer.
	*/
	void Initialize(const void *RESTRICT data[], const VkDeviceSize *dataSizes, const uint32 dataChunks) NOEXCEPT;

	/*
	*	Releases this Vulkan buffer.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer _VulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

};