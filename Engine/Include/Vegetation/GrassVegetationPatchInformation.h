#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/GridPoint2.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class GrassVegetationPatchInformation final
{

public:

	//Denotes whether or not this patch is valid.
	bool _Valid;

	//The grid point.
	GridPoint2 _GridPoint;

	//The axis aligned bounding boxes.
	StaticArray<AxisAlignedBoundingBox, UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> _AxisAlignedBoundingBoxes;

};