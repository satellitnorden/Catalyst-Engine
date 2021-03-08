#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

class ModelData final
{

public:

	//The axis aligned bounding box.
	AxisAlignedBoundingBox3D _AxisAlignedBoundingBox;

	//The number of meshes.
	uint64 _NumberOfMeshes;

	//The number of level of details.
	uint64 _NumberOfLevelfDetails;

	//The vertices of this model.
	DynamicArray<DynamicArray<DynamicArray<Vertex>>> _Vertices;

	//The indices of this model.
	DynamicArray<DynamicArray<DynamicArray<uint32>>> _Indices;

};