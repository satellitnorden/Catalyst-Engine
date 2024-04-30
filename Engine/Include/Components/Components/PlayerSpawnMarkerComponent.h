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
	);

public:

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;

	/*
	*	Returns a random world transform.
	*/
	NO_DISCARD WorldTransform GetRandomWorldTransform() NOEXCEPT;

};