#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class VegetationComponent final
{

public:

	//The descriptor set.
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