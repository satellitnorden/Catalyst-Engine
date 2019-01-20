#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationVertex.h>

class TreeVegetationModelData final
{

public:

	//The extent of the grass vegetation model.
	StaticArray<float, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Extents;

	//The vertices of the grass vegetation model.
	StaticArray<DynamicArray<VegetationVertex>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Vertices;

	//The indices of the grass vegetation model.
	StaticArray<DynamicArray<uint32>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Indices;

};