#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanBuffer.h>
#include <VulkanDescriptorSet.h>

class TerrainRenderComponent final
{

public:

	//The descriptor set.
	VulkanDescriptorSet descriptorSet{};

	//The vertex buffer.
	VulkanBuffer vertexAndIndexBuffer{};

	//The index buffer offset.
	uint32 indexBufferOffset{ 0 };

	//The index count.
	uint32 indexCount{ 0 };

};