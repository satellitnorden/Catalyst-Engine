#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class VegetationComponent final
{

public:

	//The render data table.
	RenderDataTableHandle renderDataTable;

	//The transformations buffer.
	ConstantBufferHandle transformationsBuffer;

	//Whether or not to draw a particular grid cell.
	DynamicArray<bool> shouldDrawGridCell;

	//The offsets into the transformations buffer.
	DynamicArray<uint64> transformationOffsets;

	//The number of instances.
	DynamicArray<uint32> instanceCounts;

};