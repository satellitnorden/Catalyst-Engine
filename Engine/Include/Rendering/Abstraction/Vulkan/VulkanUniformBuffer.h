#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSet.h>

class VulkanUniformBuffer final
{

public:

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return _VulkanBuffer; }

	/*
	*	Initializes this Vulkan uniform buffer.
	*/
	void Initialize(const VkDeviceSize newUniformBufferSize, const VkBufferUsageFlags usage) NOEXCEPT;

	/*
	*	Releases this Vulkan uniform buffer.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkDeviceSize& GetSize() const NOEXCEPT { return _UniformBufferSize; }

	/*
	*	Uploads new data to the uniform buffer.
	*/
	void UploadData(const void *RESTRICT newData) const NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer _VulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The size of this uniform buffer.
	VkDeviceSize _UniformBufferSize;

};
#endif