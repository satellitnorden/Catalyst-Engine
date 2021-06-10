#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//World.
#include <World/Core/WorldPosition.h>

class LightComponent final
{

public:

	union
	{
		//The rotation. Used for directional lights.
		EulerAngles _Rotation;

		//The world position. Used for point lights.
		WorldPosition _WorldPosition;
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
	FORCE_INLINE LightComponent() NOEXCEPT
		:
		_WorldPosition(VectorConstants::ZERO),
		_Color(VectorConstants::ZERO),
		_LightType(LightType::POINT),
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
				_Rotation = other._Rotation;

				break;
			}

			case LightType::POINT:
			{
				_WorldPosition = other._WorldPosition;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

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