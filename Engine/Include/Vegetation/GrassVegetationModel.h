#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class GrassVegetationModel final
{

public:

	//The model space axis-aligned bounding boxes of this physical model.
	StaticArray<AxisAlignedBoundingBox, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _AxisAlignedBoundingBoxes;

	//The buffer.
	StaticArray<BufferHandle, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Buffers;

	//The index offset in the buffer.
	StaticArray<uint64, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _IndexOffsets;

	//The number of indices that this grass model has.
	StaticArray<uint32, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _IndexCounts;

	/*
	*	Default constructor.
	*/
	GrassVegetationModel() NOEXCEPT
	{

	}

};