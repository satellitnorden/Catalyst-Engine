#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/AxisAlignedBoundingBox.h>
#include <Math/GridPoint2.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class TreeVegetationPatchInformation final
{

public:

	//Denotes whether or not this patch is valid.
	bool _Valid;

	//The grid point.
	GridPoint2 _GridPoint;

	//The transformations.
	DynamicArray<Matrix4> _Transformations;

	//The timestamp.
	uint64 _TimeStamp;

	//The axis aligned bounding boxes.
	StaticArray<AxisAlignedBoundingBox, UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails)> _AxisAlignedBoundingBoxes;

};