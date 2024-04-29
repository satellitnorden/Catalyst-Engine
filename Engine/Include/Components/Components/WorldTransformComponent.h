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

class WorldTransformComponent final : public Component
{
	//Component declaration.
	CATALYST_COMPONENT
	(
		WorldTransform,
		COMPONENT_INITIALIZE()
		COMPONENT_PARALLEL_BATCH_UPDATE(UpdatePhase::PRE, 512)
	);
};