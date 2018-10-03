#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Updateable.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/CameraEntity.h>

class SoakCamera final : public Updateable
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(SoakCamera);

	/*
	*	Default constructor.
	*/
	SoakCamera() NOEXCEPT { }

	/*
	*	Initializes the soak camera.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the soak camera asynchronously during the logic update phase.
	*/
	bool LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT final override;

private:

	//The camera.
	CameraEntity *RESTRICT _Camera;

	//The time until the properties of the soak camera is randomized again.
	float _RandomizeTime;

	//The timer until the properties of the soak camera is randomized again.
	float _RandomizeTimer{ 0.0f };

	//The current height over terrain.
	float _CurrentHeightOverTerrain;

	//Current rotation, expressed as degrees per second.
	float _CurrentRotation;

	/*
	*	Randomizes the properties of the soak camera.
	*/
	void Randomize() NOEXCEPT;

};