#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/AnimatedModelAsset.h>

//Forward declarations.
class RenderInputStream;

class AnimatedModelInitializationData final : public ComponentInitializationData
{

public:

	//The model.
	AssetPointer<AnimatedModelAsset> _Model;

};

class AnimatedModelInstanceData final
{

public:

	//The model.
	AssetPointer<AnimatedModelAsset> _Model;

};

class AnimatedModelComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		AnimatedModel,
		COMPONENT_INITIALIZE()
		COMPONENT_POST_CREATE_INSTANCE()
	);

private:

	/*
	*	Gathers the animated model input stream.
	*/
	void GatherAnimatedModelInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

};