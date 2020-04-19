#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Material.h>

//Resources.
#include <Resources/Core/AnimatedModelResource.h>
#include <Resources/Core/AnimationResource.h>
#include <Resources/Core/ResourcePointer.h>

class AnimatedModelComponent final
{

public:

	//The animated model resource.
	ResourcePointer<AnimatedModelResource> _AnimatedModelResource;

	//The material.
	Material _Material;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox3 _WorldSpaceAxisAlignedBoundingBox;

	//The previous world transform.
	Matrix4x4 _PreviousWorldTransform;

	//The current world transform.
	Matrix4x4 _CurrentWorldTransform;

	//The current animated model resource.
	ResourcePointer<AnimationResource> _CurrentAnimationResource;

	//The current animation time.
	float32 _CurrentAnimationTime{ 0.0f };

	//The animation data buffer.
	DynamicArray<BufferHandle> _AnimationDataBuffers;

	//The animation data render data table.
	DynamicArray<RenderDataTableHandle> _AnimationDataRenderDataTables;

};