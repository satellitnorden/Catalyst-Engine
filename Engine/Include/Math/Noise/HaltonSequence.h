#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace HaltonSequence
{

	/*
	*	Generates a Halton Sequence with the given parameters.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Generate(uint8 index, uint8 base) NOEXCEPT
	{
		float F{ 1.0f };
		float R{ 0.0f };

		while (index > 0)
		{
			F = F / base;
			R = R + F * (index % base);
			index = index / base;
		}

		return R;
	}

}