#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanBuffer.h>
#include <VulkanDescriptorSet.h>

class RenderComponent final
{

public:

	//The descriptor set.
	VulkanDescriptorSet descriptorSet{ };

	//The buffer.
	VulkanBuffer buffer{ };

	//The offset for the indices in the buffer.
	uint64 indexOffset{ 0 };

	//The index count.
	uint32 indexCount{ 0 };

};