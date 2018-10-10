#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/GrassVertex.h>

class GrassModelData final
{

public:

	//The vertices of the grass model.
	DynamicArray<GrassVertex> _Vertices;

	//The indices of the grass model.
	DynamicArray<uint32> _Indices;

};