#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <CameraEntity.h>

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

};