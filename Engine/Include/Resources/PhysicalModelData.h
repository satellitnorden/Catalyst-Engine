#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/PhysicalVertex.h>
#include <Rendering/Engine/RenderingCore.h>

class PhysicalModelData final
{

public:

	//The extent of the physical model.
	StaticArray<float, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Extents;

	//The vertices of the physical model.
	StaticArray<DynamicArray<PhysicalVertex>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Vertices;

	//The indices of the physical model.
	StaticArray<DynamicArray<uint32>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Indices;

};