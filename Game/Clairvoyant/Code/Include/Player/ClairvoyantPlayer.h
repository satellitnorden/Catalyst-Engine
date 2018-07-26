#pragma once

//Core.
#include <Core/EngineCore.h>

//Entities.
#include <Entities/CameraEntity.h>

//Forward declarations.
class SpotLightEntity;

class ClairvoyantPlayer final : public CameraEntity
{

public:

	/*
	*	Initializes the player.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the player.
	*/
	void Update(const float deltaTime) NOEXCEPT;

private:

	//The flashlight of this player.
	SpotLightEntity *flashlight;

	//Defines whether or not to constrain the player to the ground. 
	bool constrainToGround{ false };

};