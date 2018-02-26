#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <RenderingCore.h>

//Vulkan.
#include <VulkanBuffer.h>
#include <VulkanDescriptorSet.h>

class TerrainRenderComponent final
{

public:

	//The descriptor set.
	DescriptorSetHandle descriptorSet{ };

	//The vertex buffer.
	GraphicsBufferHandle vertexAndIndexBuffer{ };

	//The index buffer offset.
	uint32 indexBufferOffset{ 0 };

	//The index count.
	uint32 indexCount{ 0 };

};