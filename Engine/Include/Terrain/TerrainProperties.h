#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Terrain.
#include <Terrain/TerrainCore.h>

class TerrainProperties final
{

public:

	//The texture tiling factor.
	float _TerrainTextureTilingFactor;

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