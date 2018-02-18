#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <CPUTexture4.h>
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

	//The height map.
	CPUTexture4 terrainProperties;

};