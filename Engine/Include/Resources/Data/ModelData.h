#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Engine/Vertex.h>

class ModelData final
{

public:

	//The vertices of this model.
	DynamicArray<Vertex> _Vertices;

	//The indices of this model.
	DynamicArray<uint32> _Indices;

	//The extent of this model.
	float _Extent;

};