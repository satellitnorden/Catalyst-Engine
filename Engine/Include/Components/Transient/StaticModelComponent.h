#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Resources.
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

class StaticModelComponent final
{

public:

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The world transform.
	Matrix4x4 _WorldTransform;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox3 _WorldSpaceAxisAlignedBoundingBox;

	//The material indices.
	DynamicArray<uint32> _MaterialIndices;

	//The level of detail indices.
	DynamicArray<uint32> _LevelOfDetailIndices;

};