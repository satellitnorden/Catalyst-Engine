#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//World.
#include <World/Core/WorldTransform.h>

class WorldTransformInitializationData final : public ComponentInitializationData
{

public:

	//The world transform.
	WorldTransform _WorldTransform;

};

class WorldTransformInstanceData final
{

public:

	//The previous world transform.
	WorldTransform _PreviousWorldTransform;

	//The current world transform.
	WorldTransform _CurrentWorldTransform;

};

DECLARE_COMPONENT
(
	WorldTransformComponent,
	WorldTransformInitializationData,
	WorldTransformInstanceData,
	
public:

	/*
	*	Initializes this component.
	*/
	void Initialize() NOEXCEPT override;
	
);