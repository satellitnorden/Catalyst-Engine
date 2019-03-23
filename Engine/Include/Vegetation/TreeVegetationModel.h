#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TreeVegetationModel final
{

public:

	//The model space axis-aligned bounding box of this tree vegetation model.
	StaticArray<AxisAlignedBoundingBox, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _AxisAlignedBoundingBoxes;

	//The buffers for the crown.
	StaticArray<ConstantBufferHandle, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _CrownBuffers;

	//The index offset in the buffers for the crown.
	StaticArray<uint64, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _CrownIndexOffsets;

	//The number of indices that this grass vegetation model has for the crown.
	StaticArray<uint32, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _CrownIndexCounts;

	//The buffers for the trunk.
	StaticArray<ConstantBufferHandle, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TrunkBuffers;

	//The index offset in the buffers for the trunk.
	StaticArray<uint64, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TrunkIndexOffsets;

	//The number of indices that this grass vegetation model has for the trunk.
	StaticArray<uint32, UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails)> _TrunkIndexCounts;

	/*
	*	Default constructor.
	*/
	TreeVegetationModel() NOEXCEPT
	{

	}

};