#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanDescriptorSet.h>
#include <VulkanIndexBuffer.h>
#include <VulkanVertexBuffer.h>

class RenderComponent final
{

public:

	//The descriptor set.
	VulkanDescriptorSet descriptorSet{ };

	//The vertex buffer.
	VulkanVertexBuffer vertexBuffer{ };

	//The index buffer.
	VulkanIndexBuffer indexBuffer{ };

	//The index count.
	uint32 indexCount{ 0 };

};