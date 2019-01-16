#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationVertex.h>

class TreeVegetationModelData final
{

public:

	//The extent of the grass vegetation model.
	float _Extent;

	//The vertices of the grass vegetation model.
	DynamicArray<VegetationVertex> _Vertices;

	//The indices of the grass vegetation model.
	DynamicArray<uint32> _Indices;

};