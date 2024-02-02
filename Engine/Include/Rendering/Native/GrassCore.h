#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Grass constants.
namespace GrassConstants
{
	constexpr float32 LEVEL_OF_DETAIL_DISTANCE_FACTOR{ 8.0f };
	constexpr uint8 HIGHEST_LEVEL_OF_DETAIL{ 4 };
	constexpr uint32 HIGHEST_VERTICES{ 11 };
}

/*
*	Grass instance class definition.
*/
class GrassInstance final
{

public:

	//The position.
	Vector3<float32> _Position;

	//The rotation.
	float32 _Rotation;

};