#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/TerrainUniformData.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanUniformBuffer.h>

class TerrainComponent final
{

public:

	//The terrain uniform data.
	TerrainUniformData terrainUniformData;

	//The uniform buffer.
	UniformBufferHandle uniformBuffer;

	//The height map.
	CPUTexture2D terrainProperties;

	//The terrain properties texture.
	Texture2DHandle terrainPropertiesTexture;

};