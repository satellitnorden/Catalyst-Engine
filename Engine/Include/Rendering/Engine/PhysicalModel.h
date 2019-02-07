#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/RenderingCore.h>

class PhysicalModel final
{

public:

	//The model space axis-aligned bounding boxes of this physical model.
	StaticArray<AxisAlignedBoundingBox, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _AxisAlignedBoundingBoxes;

	//The buffers.
	StaticArray<ConstantBufferHandle, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _Buffers;

	//The index offsets in the buffer.
	StaticArray<uint64, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _IndexOffsets;

	//The number of indices that this physical model has.
	StaticArray<uint32, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _IndexCounts;

	/*
	*	Default constructor.
	*/
	PhysicalModel() NOEXCEPT
	{

	}

};