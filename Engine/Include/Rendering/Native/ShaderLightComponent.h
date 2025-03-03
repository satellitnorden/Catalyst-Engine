#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Entities.
#include <Entities/Core/Entity.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class ShaderLightComponent final
{

public:

	union
	{
		//The direction. Used for directional lights.
		Vector3<float32> _Direction;

		//The world position. Used for point lights.
		Vector3<float32> _WorldPosition;

		struct
		{
			//The minimum world position. Used for box lights.
			Vector3<float32> _MinimumWorldPosition;

			//The maximum world position. Used for box lights.
			Vector3<float32> _MaximumWorldPosition;
		};
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
	Padding<8> _Padding;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ShaderLightComponent() NOEXCEPT
		:
		_WorldPosition(VectorConstants::ZERO),
		_Color(VectorConstants::ZERO),
		_LightType(static_cast<uint32>(LightType::POINT)),
		_LightProperties(0),
		_Intensity(0.0f),
		_Radius(0.0f),
		_Size(0.0f)
	{

	}

	/*
	*	Copy by LightComponent constructor.
	*/
	ShaderLightComponent(Entity *const RESTRICT entity) NOEXCEPT;

};

static_assert(sizeof(ShaderLightComponent) == 64, "Light components must be exactly 64 bytes!");