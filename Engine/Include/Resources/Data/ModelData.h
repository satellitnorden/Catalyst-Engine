#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Engine/Vertex.h>

class ModelData final
{

public:

	//The axis-aligned bounding box.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The vertices of this model.
	DynamicArray<Vertex> _Vertices;

	//The indices of this model.
	DynamicArray<uint32> _Indices;

};