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

	//The light type.
	LightType _LightType;

	union
	{
		//The position. Used for point lights.
		Vector3<float> _Position;

		//The direction. Used for directional lights.
		Vector3<float> _Direction;
	};

	//The luminance.
	Vector3<float> _Luminance;

	//The size.
	float _Size;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LightInitializationData() NOEXCEPT
		:
		_LightType(LightType::POINT),
		_Position(VectorConstants::ZERO),
		_Luminance(VectorConstants::ZERO),
		_Size(0.0f)
	{

	}

};