#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace CatalystToneMapping
{

	/*
	*	Applies tone mapping.
	*/
	FORCE_INLINE NO_DISCARD Vector3<float> ApplyToneMapping(Vector3<float> color) NOEXCEPT
	{
		constexpr float GAMMA{ 1.0f / 2.2f };
		constexpr float A{ 2.51f };
		constexpr float B{ 0.03f };
		constexpr float C{ 2.43f };
		constexpr float D{ 0.59f };
		constexpr float E{ 0.14f };

		color *= 0.6f;

		Vector3<float> tone_mapped{ (color * (A * color + B)) / (color * (C * color + D) + E) };

		tone_mapped._R = pow(tone_mapped._R, GAMMA);
		tone_mapped._G = pow(tone_mapped._G, GAMMA);
		tone_mapped._B = pow(tone_mapped._B, GAMMA);

		return tone_mapped;
	}

}