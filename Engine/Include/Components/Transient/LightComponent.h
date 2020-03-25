#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class LightComponent final
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
	uint32 _LightType;

	//The light properties.
	uint32 _LightProperties;

	//The intensity.
	float32 _Intensity;

	//The radius.
	float32 _Radius;

	//The size.
	float32 _Size;

	//Padding.
	Padding<4> _Padding;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LightComponent() NOEXCEPT
		:
		_Position(VectorConstants::ZERO),
		_Color(VectorConstants::ZERO),
		_LightType(static_cast<uint32>(LightType::POINT)),
		_LightProperties(0),
		_Intensity(0.0f),
		_Radius(0.0f),
		_Size(0.0f)
	{

	}

	/*
	*	Copy operator overload.
	*/
	void operator=(const LightComponent& other) NOEXCEPT
	{
		switch (static_cast<LightType>(other._LightType))
		{
			case LightType::DIRECTIONAL:
			{
				_Direction = other._Direction;

				break;
			}

			case LightType::POINT:
			{
				_Position = other._Position;

				break;
			}
		}

		_Color = other._Color;
		_LightType = other._LightType;
		_LightProperties = other._LightProperties;
		_Intensity = other._Intensity;
		_Radius = other._Radius;
		_Size = other._Size;
	}

};

static_assert(sizeof(LightComponent) == 48, "Light components must be exactly 48 bytes!");