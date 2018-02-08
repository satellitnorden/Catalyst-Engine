#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <TerrainUniformData.h>

//Vulkan.
#include <VulkanUniformBuffer.h>

class TerrainComponent final
{

public:

	//The terrain uniform data.
	TerrainUniformData terrainUniformData;

	//The uniform buffer.
	VulkanUniformBuffer uniformBuffer;

};