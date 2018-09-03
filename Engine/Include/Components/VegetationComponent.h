#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class VegetationComponent final
{

public:

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The transformations buffer.
	ConstantBufferHandle _TransformationsBuffer;

	//Whether or not to draw a particular grid cell.
	DynamicArray<bool> _ShouldDrawGridCell;

	//The offsets into the transformations buffer.
	DynamicArray<uint64> _TransformationOffsets;

	//The number of instances.
	DynamicArray<uint32> _InstanceCounts;

};