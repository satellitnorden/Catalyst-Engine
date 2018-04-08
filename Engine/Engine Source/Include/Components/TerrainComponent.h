#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture2D.h>
#include <Rendering/Engine Layer/RenderingCore.h>
#include <Rendering/Engine Layer/TerrainUniformData.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanUniformBuffer.h>

class TerrainComponent final
{

public:

	//The terrain uniform data.
	TerrainUniformData terrainUniformData;

	//The uniform buffer.
	UniformBufferHandle uniformBuffer;

	//The height map.
	CPUTexture2D terrainProperties;

};