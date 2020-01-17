#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

class ModelData final
{

public:

	//The number of level of details.
	uint64 _NumberOfLevelfDetails;

	//The axis-aligned bounding box.
	DynamicArray<AxisAlignedBoundingBox> _AxisAlignedBoundingBoxes;

	//The vertices of this model.
	DynamicArray<DynamicArray<Vertex>> _Vertices;

	//The indices of this model.
	DynamicArray<DynamicArray<uint32>> _Indices;

};