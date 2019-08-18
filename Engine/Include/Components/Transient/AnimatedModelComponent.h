#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Animation.
#include <Animation/AnimatedModel.h>

//Rendering.
#include <Rendering/Native/Material.h>

class AnimatedModelComponent final
{

public:

	//The model.
	const AnimatedModel *RESTRICT _Model{ nullptr };

	//The previous world transform.
	Matrix4 _PreviousWorldTransform;

	//The current world transform.
	Matrix4 _CurrentWorldTransform;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox _WorldSpaceAxisAlignedBoundingBox;

	//The material.
	Material _Material;

	//The animation data buffer.
	DynamicArray<BufferHandle> _AnimationDataBuffers;

	//The animation data render data table.
	DynamicArray<RenderDataTableHandle> _AnimationDataRenderDataTables;

};