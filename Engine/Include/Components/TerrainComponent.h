#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

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
	TerrainUniformData _TerrainUniformData;

	//The height map.
	CPUTexture2D _TerrainProperties;

	//The terrain properties texture.
	Texture2DHandle _TerrainPropertiesTexture;

	//The uniform buffer.
	UniformBufferHandle _UniformBuffer;

};