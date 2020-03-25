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

	//The luminance.
	Vector3<float32> _Luminance;

	//The light type.
	uint32 _LightType;

	//The size.
	float32 _Size;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LightComponent() NOEXCEPT
		:
		_Position(VectorConstants::ZERO),
		_Luminance(VectorConstants::ZERO),
		_LightType(static_cast<uint32>(LightType::POINT)),
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

		_Luminance = other._Luminance;
		_LightType = other._LightType;
		_Size = other._Size;
	}

};

static_assert(sizeof(LightComponent) == 32, "Light components must be exactly 32 bytes!");