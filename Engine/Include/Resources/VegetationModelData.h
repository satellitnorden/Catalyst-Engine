#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/VegetationVertex.h>

class VegetationModelData final
{

public:

	//The vertices of the vegetation model.
	DynamicArray<VegetationVertex> _Vertices;

	//The indices of the vegetation model.
	DynamicArray<uint32> _Indices;

};