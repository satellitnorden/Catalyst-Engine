#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Animation.
#include <Animation/Animation.h>
#include <Animation/AnimatedModel.h>

//Rendering.
#include <Rendering/Native/Material.h>

class AnimatedModelComponent final
{

public:

	//The model.
	const AnimatedModel *RESTRICT _Model{ nullptr };

	//The material.
	Material _Material;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox3 _WorldSpaceAxisAlignedBoundingBox;

	//The previous world transform.
	Matrix4x4 _PreviousWorldTransform;

	//The current world transform.
	Matrix4x4 _CurrentWorldTransform;

	//The current animation.
	const Animation *RESTRICT _CurrentAnimation{ nullptr };

	//The current animation time.
	float _CurrentAnimationTime{ 0.0f };

	//The animation data buffer.
	DynamicArray<BufferHandle> _AnimationDataBuffers;

	//The animation data render data table.
	DynamicArray<RenderDataTableHandle> _AnimationDataRenderDataTables;

};