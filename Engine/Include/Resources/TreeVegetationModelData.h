#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationVertex.h>

class TreeVegetationModelData final
{

public:

	//The extent for the crown of the grass vegetation model.
	StaticArray<float, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _CrownExtents;

	//The vertices for the crown of the grass vegetation model.
	StaticArray<DynamicArray<VegetationVertex>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _CrownVertices;

	//The indices for the crown of the grass vegetation model.
	StaticArray<DynamicArray<uint32>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _CrownIndices;

	//The extent for the trunk of the grass vegetation model.
	StaticArray<float, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TrunkExtents;

	//The vertices for the trunk of the grass vegetation model.
	StaticArray<DynamicArray<VegetationVertex>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TrunkVertices;

	//The indices for the trunk of the grass vegetation model.
	StaticArray<DynamicArray<uint32>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TrunkIndices;

};