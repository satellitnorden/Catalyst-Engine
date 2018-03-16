#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Entities.
#include <Entities/CameraEntity.h>

//Forward declarations.
class SpotLightEntity;

class ClairvoyantPlayer final : public CameraEntity
{

public:

	/*
	*	Default constructor.
	*/
	ClairvoyantPlayer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~ClairvoyantPlayer() NOEXCEPT;

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