#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/TerrainMaterial.h>

//Terrain.
#include <Terrain/TerrainDisplacementInformation.h>

class TerrainPatchInformation final
{

public:

	//The grid X position.
	int64 _X;

	//The grid Y position.
	int64 _Y;

	//The normal/height map.
	CPUTexture2D _NormalHeightMap;

	//The layer weights map.
	CPUTexture2D _LayerWeightsMap;

	//The material.
	TerrainMaterial _Material;

	//The displacement information.
	TerrainDisplacementInformation _DisplacementInformation;

};