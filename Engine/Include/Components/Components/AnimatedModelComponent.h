#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class AnimatedModelInitializationData final : public ComponentInitializationData
{

public:

	//The animated model resource.
	//ResourcePointer<AnimatedModelResource> _AnimatedModelResource;

	//The material resource.
	//ResourcePointer<MaterialResource> _MaterialResource;

	//The initial animation resource.
	//ResourcePointer<AnimationResource> _InitialAnimationResource;

};

class AnimatedModelInstanceData final
{

public:

	//The animated model resource.
	//ResourcePointer<AnimatedModelResource> _AnimatedModelResource;

	//The material resource.
	//ResourcePointer<MaterialResource> _MaterialResource;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The current animated model resource.
	//ResourcePointer<AnimationResource> _CurrentAnimationResource;

	//The current animation time.
	float32 _CurrentAnimationTime{ 0.0f };

	//The animation data buffer.
	DynamicArray<BufferHandle> _AnimationDataBuffers;

	//The animation data render data table.
	DynamicArray<RenderDataTableHandle> _AnimationDataRenderDataTables;

};

class AnimatedModelComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		AnimatedModel
	);

public:

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity* const RESTRICT entity) NOEXCEPT override;

};