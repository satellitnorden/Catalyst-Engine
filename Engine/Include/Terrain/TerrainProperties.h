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

	//The render data table for the terrain material.
	RenderDataTableHandle _RenderDataTable;

	//The resolution of each patch.
	uint32 _PatchResolution;

	//The size of each patch.
	float _PatchSize;

	//The height generation function.
	HeightGenerationFunction _HeightGenerationFunction;

	//The layer weights generation function.
	LayerWeightsGenerationFunction _LayerWeightsGenerationFunction;

	//The patch properties generation function.
	PatchPropertiesGenerationFunction _PatchPropertiesGenerationFunction;

};