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

	//The mass, denoted in kilograms.
	float _Mass{ 100.0f };

	//The height, denoted in meters.
	float _Height{ 1.75f };

	//The radius, denoted in meters.
	float _Radius{ 0.25f };

	//Denotes whether or not character movement is enabled.
	bool _Enabled{ true };

	/*
	*	Adds movement in a direction.
	*/
	void AddMovement(const Vector3<float> &direction, const float force) NOEXCEPT
	{
		_Velocity += direction * force;
	}

};