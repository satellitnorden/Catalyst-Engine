#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class TerrainVertex final
{

public:

	//The position on the X axis.
	float _PositionX;

	//The position on the Y axis.
	float _PositionY;

	//The position on the Z axis.
	float _PositionZ;

	//The normal on the X axis.
	float _NormalX;

	//The normal on the Y axis.
	float _NormalY;

	//The normal on the Z axis.
	float _NormalZ;

	//The layer weight on the X axis.
	float _LayerWeightX;

	//The layer weight on the Y axis.
	float _LayerWeightY;

	//The layer weight on the Z axis.
	float _LayerWeightZ;

	//The layer weight on the W axis.
	float _LayerWeightW;

};