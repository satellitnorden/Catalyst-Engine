#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Material.h>
#include <Rendering/Native/Model.h>

class DynamicModelComponent final
{

public:

	//The model.
	const Model *RESTRICT _Model{ nullptr };

	//The previous world transform.
	Matrix4 _PreviousWorldTransform;

	//The current world transform.
	Matrix4 _CurrentWorldTransform;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox _WorldSpaceAxisAlignedBoundingBox;

	//The material.
	Material _Material;

};