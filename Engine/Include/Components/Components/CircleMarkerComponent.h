#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

class CircleMarkerInitializationData final : public ComponentInitializationData
{

public:

	//The radius.
	float32 _Radius{ 0.5f };

};

class CircleMarkerInstanceData final
{

public:

	//The radius.
	float32 _Radius;

};

DECLARE_COMPONENT
(
	CircleMarkerComponent,
	CircleMarkerInitializationData,
	CircleMarkerInstanceData,

public:

	/*
	*	Initializes this component.
	*/
	void Initialize() NOEXCEPT override;

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;

);