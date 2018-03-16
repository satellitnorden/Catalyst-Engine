#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSet.h>

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