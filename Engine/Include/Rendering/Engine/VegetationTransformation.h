#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector2.h>
#include <Math/Vector3.h>

class VegetationTransformation final
{

public:

	//The position.
	Vector3 position;

	//The scale.
	Vector2 scale;

	//The rotation.
	float rotation;

	/*
	*	Constructor taking all values as arguments.
	*/
	VegetationTransformation(const Vector3 &initialPosition, const Vector2 &initialScale, const float initialRotation) NOEXCEPT
		:
		position(initialPosition),
		scale(initialScale),
		rotation(initialRotation)
	{

	}

};