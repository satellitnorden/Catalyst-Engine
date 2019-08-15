#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

class AnimatedModelData final
{

public:

	//The axis-aligned bounding box.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The vertices of this model.
	DynamicArray<AnimatedVertex> _Vertices;

	//The indices of this model.
	DynamicArray<uint32> _Indices;

};