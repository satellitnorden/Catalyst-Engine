#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Updateable.h>
#include <Core/Pointers/UniquePointer.h>

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

	//The time until the properties of the soak camera is randomized again.
	float _RandomizeTime;

	//The timer until the properties of the soak camera is randomized again.
	float _RandomizeTimer{ 0.0f };

	//The current speed.
	float _CurrentSpeed{ 0.0f };

	//The target speed.
	float _TargetSpeed;

	//The current upwards velocity.
	float _CurrentUpwardsVelocity{ 0.0f };

	//The target altitude.
	float _TargetAltitude;

	//The current rotation, expressed as degrees per second.
	float _CurrentRotation{ 0.0f };

	//The target rotation, expressed as degrees per second.
	float _TargetRotation;

	/*
	*	Updates the forward movement of the camera.
	*/
	void UpdateForwardMovement(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Updates the altitude of the camera.
	*/
	void UpdateAltitude(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Updates the rotation of the camera.
	*/
	void UpdateRotation(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Randomizes the properties of the soak camera.
	*/
	void Randomize() NOEXCEPT;

};