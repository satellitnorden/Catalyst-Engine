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
	float _Height{ 2.0f };

	//The radius, denoted in meters.
	float _Radius{ 0.25f };

	//Denotes whether or not character movement is enabled.
	bool _Enabled{ true };

	//Denotes whether or not this character is currently jumping.
	bool _IsJumping{ false };

	/*
	*	Adds movement input.
	*/
	void AddMovementInput(const Vector3<float> &input) NOEXCEPT
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