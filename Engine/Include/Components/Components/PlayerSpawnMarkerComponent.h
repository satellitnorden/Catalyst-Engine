#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//World.
#include <World/Core/WorldTransform.h>

class PlayerSpawnMarkerInitializationData final : public ComponentInitializationData
{

public:

};

class PlayerSpawnMarkerInstanceData final
{

public:

};

class PlayerSpawnMarkerComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		PlayerSpawnMarker,
		COMPONENT_INITIALIZE()
		COMPONENT_POST_CREATE_INSTANCE()
		COMPONENT_SELECT()
	);

public:

	/*
	*	Returns a random world transform.
	*/
	NO_DISCARD WorldTransform GetRandomWorldTransform() NOEXCEPT;

};