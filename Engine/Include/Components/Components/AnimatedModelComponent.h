#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Resources.
#include <Resources/Core/AnimatedModelResource.h>
#include <Resources/Core/AnimationResource.h>
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class AnimatedModelSharedData final
{

};

class AnimatedModelInitializationData final : public ComponentInitializationData
{

public:

	//The animated model resource.
	ResourcePointer<AnimatedModelResource> _AnimatedModelResource;

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

	//The initial animation resource.
	ResourcePointer<AnimationResource> _InitialAnimationResource;

};

class AnimatedModelInstanceData final
{

public:

	//The animated model resource.
	ResourcePointer<AnimatedModelResource> _AnimatedModelResource;

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The current animated model resource.
	ResourcePointer<AnimationResource> _CurrentAnimationResource;

	//The current animation time.
	float32 _CurrentAnimationTime{ 0.0f };

	//The animation data buffer.
	DynamicArray<BufferHandle> _AnimationDataBuffers;

	//The animation data render data table.
	DynamicArray<RenderDataTableHandle> _AnimationDataRenderDataTables;

};

DECLARE_COMPONENT(AnimatedModelComponent, AnimatedModelSharedData, AnimatedModelInitializationData, AnimatedModelInstanceData);