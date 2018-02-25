#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Matrix4.h>

//Vulkan.
#include <VulkanBuffer.h>
#include <VulkanDescriptorSet.h>

class StaticPhysicalRenderComponent final
{

public:

	//The model matrix.
	Matrix4 modelMatrix;

	//The descriptor set.
	VulkanDescriptorSet descriptorSet{ };

	//The buffer.
	VulkanBuffer buffer{};

	//The offset for the indices in the buffer.
	uint64 indexOffset{ 0 };

	//The index count.
	uint32 indexCount{ 0 };

};