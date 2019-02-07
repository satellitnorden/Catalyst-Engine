#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vegetation.
#include <Vegetation/VegetationVertex.h>

class GrassVegetationModelData final
{

public:

	//The extent of the grass vegetation model.
	float _Extent;

	//The vertices of the grass vegetation model.
	DynamicArray<VegetationVertex> _Vertices;

	//The indices of the grass vegetation model.
	DynamicArray<uint32> _Indices;

};