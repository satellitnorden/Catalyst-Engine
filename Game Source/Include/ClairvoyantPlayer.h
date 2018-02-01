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
	ClairvoyantPlayer() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~ClairvoyantPlayer() CATALYST_NOEXCEPT;

	/*
	*	Initializes the player.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Updates the player.
	*/
	void Update(const float deltaTime) CATALYST_NOEXCEPT;

private:

	//The flashlight of this player.
	SpotLightEntity *flashlight;

};