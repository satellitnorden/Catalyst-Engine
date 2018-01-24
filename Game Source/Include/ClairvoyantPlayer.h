#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <Entity.h>

//Forward declarations.
class CameraEntity;
class SpotLightEntity;

class ClairvoyantPlayer final : public Entity
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

	//The camera of the player.
	CameraEntity *camera{ nullptr };

	//The flashlight of this player.
	SpotLightEntity *flashlight;

};