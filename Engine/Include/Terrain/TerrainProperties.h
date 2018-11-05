#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Terrain.
#include <Terrain/TerrainCore.h>

class TerrainProperties final
{

public:

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The index offset.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The render data table for the terrain material.
	RenderDataTableHandle _RenderDataTable;

	//The height generation function.
	HeightGenerationFunction _HeightGenerationFunction;

	//The layer weights generation function.
	LayerWeightsGenerationFunction _LayerWeightsGenerationFunction;

	//The patch properties generation function.
	PatchPropertiesGenerationFunction _PatchPropertiesGenerationFunction;

};