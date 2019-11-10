#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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
		Vector3<float> _Direction;

		//The position. Used for point lights.
		Vector3<float> _Position;
	};

	//The luminance.
	Vector3<float> _Luminance;

	//The light type.
	LightType _LightType;

	//The size.
	float _Size;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LightComponent() NOEXCEPT
		:
		_Position(VectorConstants::ZERO),
		_Luminance(VectorConstants::ZERO),
		_LightType(LightType::POINT),
		_Size(0.0f)
	{

	}

	/*
	*	Copy operator overload.
	*/
	void operator=(const LightComponent& other) NOEXCEPT
	{
		switch (other._LightType)
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

		_Luminance = other._Luminance;
		_LightType = other._LightType;
		_Size = other._Size;
	}

};

static_assert(sizeof(LightComponent) == 32, "Light components bust be exactly 32 bytes!");