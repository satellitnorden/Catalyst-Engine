#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>

class VegetationTransformation final
{

public:

	//The position.
	Vector3 position;

	//The scale.
	Vector2 scale;

	//The cosine rotation.
	float cosineRotation;

	//The sine rotation.
	float sineRotation;

	/*
	*	Constructor taking all values as arguments.
	*/
	VegetationTransformation(const Vector3 &initialPosition, const Vector2 &initialScale, const float initialRotation) NOEXCEPT
		:
		position(initialPosition),
		scale(initialScale),
		cosineRotation(CatalystMath::CosineDegrees(initialRotation)),
		sineRotation(CatalystMath::SineDegrees(initialRotation))
	{

	}

};