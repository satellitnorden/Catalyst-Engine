#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/Material.h>
#include <Rendering/Native/Model.h>

class StaticModelComponent final
{

public:

	//The model.
	const Model *RESTRICT _Model{ nullptr };

	//The world transform.
	Matrix4x4 _WorldTransform;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox3 _WorldSpaceAxisAlignedBoundingBox;

	//The material indices.
	DynamicArray<uint32> _MaterialIndices;

	//The level of detail indices.
	DynamicArray<uint32> _LevelOfDetailIndices;

};