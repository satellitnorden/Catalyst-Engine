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
	Vector3 _Position;

	//The scale.
	Vector2 _Scale;

	//The rotation.
	float _Rotation;

	/*
	*	Constructor taking all values as arguments.
	*/
	VegetationTransformation(const Vector3 &initialPosition, const Vector2 &initialScale, const float initialRotation) NOEXCEPT
		:
		_Position(initialPosition),
		_Scale(initialScale),
		_Rotation(initialRotation)
	{

	}

};