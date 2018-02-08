#pragma once

//Engine core.
#include <EngineCore.h>

class Vector2 final
{

public:

	//The X component.
	float X;

	//The Y component.
	float Y;

	/*
	*	Default constructor.
	*/
	Vector2() CATALYST_NOEXCEPT;

	/*
	*	Constructor taking X and Y arguments.
	*/
	Vector2(const float newX, const float newY) CATALYST_NOEXCEPT;

	/*
	*	Returns a pointer to this vector's data.
	*/
	CATALYST_RESTRICTED float* Data() CATALYST_NOEXCEPT { return &X; }

	/*
	*	Returns the length of the vector.
	*/
	float Length() const CATALYST_NOEXCEPT;

	/*
	*	Returns the length of the vector squared.
	*/
	float LengthSquared() const  CATALYST_NOEXCEPT { return (X * X) + (Y * Y); }

	/*
	*	Normalize the vector to a unit vector.
	*/
	void Normalize() CATALYST_NOEXCEPT;

	/*
	*	Given an amount of degrees, rotate the vector.
	*/
	void Rotate(const float degrees) CATALYST_NOEXCEPT;

};