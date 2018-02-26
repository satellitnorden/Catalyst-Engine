#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <CPUTexture4.h>
#include <RenderingCore.h>
#include <TerrainUniformData.h>

//Vulkan.
#include <VulkanUniformBuffer.h>

class TerrainComponent final
{

public:

	//The terrain uniform data.
	TerrainUniformData terrainUniformData;

	//The uniform buffer.
	UniformBufferHandle uniformBuffer;

	//The height map.
	CPUTexture4 terrainProperties;

};