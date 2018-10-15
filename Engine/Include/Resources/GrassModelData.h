#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/GrassVegetationVertex.h>

class GrassModelData final
{

public:

	//The vertices of the grass vegetation model.
	DynamicArray<GrassVegetationVertex> _Vertices;

	//The indices of the grass vegetation model.
	DynamicArray<uint32> _Indices;

};