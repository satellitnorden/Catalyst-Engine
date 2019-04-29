#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Vector.h>

class LightInitializationData final : public EntityInitializationData
{

public:

	//The color.
	Vector3<float> _Color;

	//The position.
	Vector3<float> _Position;

	//The size.
	float _Size;

	//The strength.
	float _Strength;

};