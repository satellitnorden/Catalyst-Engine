#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector3.h>

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
	*	Copy constructor.
	*/
	WaterUniformData(const WaterUniformData &otherWaterUniformData) NOEXCEPT
		:
		waterHeight(otherWaterUniformData.waterHeight),
		waterSize(otherWaterUniformData.waterSize),
		waterWorldPosition(otherWaterUniformData.waterWorldPosition)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	WaterUniformData(const float initialWaterHeight, const float initialWaterSize, const Vector3 &initialWaterWorldPosition) NOEXCEPT
		:
		waterHeight(initialWaterHeight),
		waterSize(initialWaterSize),
		waterWorldPosition(initialWaterWorldPosition)
	{

	}

	//The water height.
	float waterHeight;

	//The water size.
	float waterSize;

	//Padding.
	Padding<8> padding;

	//The water world position.
	Vector3 waterWorldPosition;

};