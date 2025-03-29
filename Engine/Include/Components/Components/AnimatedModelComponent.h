#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/AnimatedModelAsset.h>
#include <Content/Assets/AnimationAsset.h>

//Forward declarations.
class RenderInputStream;

class AnimatedModelInitializationData final : public ComponentInitializationData
{

public:

	//The model.
	AssetPointer<AnimatedModelAsset> _Model;

	//The initial animation.
	AssetPointer<AnimationAsset> _InitialAnimation;

};

class AnimatedModelInstanceData final
{

public:

	//The model.
	AssetPointer<AnimatedModelAsset> _Model;

	//The current animation.
	AssetPointer<AnimationAsset> _CurrentAnimation;

	//The final bone transforms.
	DynamicArray<Matrix4x4> _FinalBoneTransforms;

	//The start bone transform.
	uint32 _StartBoneTransform;

};

class AnimatedModelComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		AnimatedModel,
		COMPONENT_INITIALIZE()
		COMPONENT_POST_INITIALIZE()
		COMPONENT_POST_CREATE_INSTANCE()
		COMPONENT_PARALLEL_BATCH_UPDATE(UpdatePhase::PRE_RENDER, 64)
		COMPONENT_POST_UPDATE(UpdatePhase::PRE_RENDER)
	);

private:

	//The final bone transforms.
	DynamicArray<Matrix4x4> _FinalBoneTransforms;

	/*
	*	Gathers the animated model input stream.
	*/
	void GatherAnimatedModelInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

};