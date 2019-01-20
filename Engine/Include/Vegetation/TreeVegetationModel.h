#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/RenderingCore.h>

class TreeVegetationModel final
{

public:

	//The model space axis-aligned bounding box of this tree vegetation model.
	StaticArray<AxisAlignedBoundingBox, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _AxisAlignedBoundingBoxes;

	//The buffer.
	StaticArray<ConstantBufferHandle, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Buffers;

	//The index offset in the buffer.
	StaticArray<uint64, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _IndexOffsets;

	//The number of indices that this grtree vegetationass model has.
	StaticArray<uint32, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _IndexCounts;

	/*
	*	Default constructor.
	*/
	TreeVegetationModel() NOEXCEPT
	{

	}

};