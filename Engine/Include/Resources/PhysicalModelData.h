#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/PhysicalVertex.h>

class PhysicalModelData final
{

public:

	//The extent of the physical model.
	float extent;

	//The vertices of the physical model.
	DynamicArray<PhysicalVertex> vertices;

	//The indices of the physical model.
	DynamicArray<uint32> indices;

};