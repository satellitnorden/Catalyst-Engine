#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/PhysicalVertex.h>

class PhysicalModelData final
{

public:

	//The extent of the physical model.
	float _Extent;

	//The vertices of the physical model.
	DynamicArray<PhysicalVertex> _Vertices;

	//The indices of the physical model.
	DynamicArray<uint32> _Indices;

};