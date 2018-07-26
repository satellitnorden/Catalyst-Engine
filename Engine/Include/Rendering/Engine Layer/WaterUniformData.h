#pragma once

//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

class WaterUniformData final
{

public:

	/*
	*	Default constructor.
	*/
	WaterUniformData() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	WaterUniformData(const float initialWaterSize, const float initialWaterTextureScalingFactor, const Vector3 &initialWaterWorldPosition) NOEXCEPT
		:
		waterSize(initialWaterSize),
		waterTextureScalingFactor(initialWaterTextureScalingFactor),
		waterWorldPosition(initialWaterWorldPosition)
	{

	}

	//The water size.
	float waterSize;

	//The water texture scaling factor.
	float waterTextureScalingFactor;

	//Padding.
	Padding<8> padding;

	//The water world position.
	Vector3 waterWorldPosition;

};