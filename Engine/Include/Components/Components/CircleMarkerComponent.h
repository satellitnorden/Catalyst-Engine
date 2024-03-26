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

);