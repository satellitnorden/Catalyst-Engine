#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class LightInitializationData final : public EntityInitializationData
{

public:

	union
	{
		//The direction. Used for directional lights.
		Vector3<float32> _Direction;

		//The position. Used for point lights.
		Vector3<float32> _Position;
	};

	//The color.
	Vector3<float32> _Color;

	//The light type.
	LightType _LightType;

	//The light properties.
	uint32 _LightProperties;

	//The intensity.
	float32 _Intensity;

	//The radius.
	float32 _Radius;

	//The size.
	float32 _Size;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LightInitializationData() NOEXCEPT
		:
		_Position(VectorConstants::ZERO),
		_Color(VectorConstants::ZERO),
		_LightType(LightType::POINT),
		_LightProperties(0),
		_Intensity(0.0f),
		_Radius(0.0f),
		_Size(0.0f)
	{

	}

};