#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/Vertex.h>

class PhysicalModelData final
{

public:

	//The extent of the physical model.
	StaticArray<float, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Extents;

	//The vertices of the physical model.
	StaticArray<DynamicArray<Vertex>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Vertices;

	//The indices of the physical model.
	StaticArray<DynamicArray<uint32>, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Indices;

};