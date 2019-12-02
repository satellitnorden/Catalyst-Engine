#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class CharacterMovement final
{

public:

	//The position.
	Vector3<float> _Position{ VectorConstants::ZERO };

	//The velocity.
	Vector3<float> _Velocity{ VectorConstants::ZERO };

	//The movement input.
	Vector3<float> _MovementInput{ VectorConstants::ZERO };

	//The jump input.
	float _JumpInput{ 0.0f };

	//The mass, denoted in kilograms.
	float _Mass{ 100.0f };

	//The height, denoted in meters.
	float _Height{ 1.75f };

	//The radius, denoted in meters.
	float _Radius{ 0.5f };

	//Denotes whether or not character movement is enabled.
	bool _Enabled{ true };

	/*
	*	Adds movement input.
	*/
	void AddMovementInput(const Vector3<float> & input) NOEXCEPT
	{
		_MovementInput += input;
	}

	/*
	*	Adds jump input.
	*/
	void AddJumpInput(const float input) NOEXCEPT
	{
		_JumpInput += input;
	}

};