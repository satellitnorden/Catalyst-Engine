#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/GrassVegetationVertex.h>

class GrassVegetationModelData final
{

public:

	//The extent of the grass vegetation model.
	float _Extent;

	//The vertices of the grass vegetation model.
	DynamicArray<GrassVegetationVertex> _Vertices;

	//The indices of the grass vegetation model.
	DynamicArray<uint32> _Indices;

};