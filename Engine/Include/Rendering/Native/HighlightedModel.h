#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class HighlightedModel final
{

public:

	//The highlight color.
	Vector3<float> _HighlightColor;

	//The components index
	uint64 _ComponentsIndex;

	//The highlight strength.
	float _HighlightStrength;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr HighlightedModel(const Vector3<float>& initial_highlight_color,
											const uint64 initial_components_index,
											const float initial_highlight_strength) NOEXCEPT
		:
		_HighlightColor(initial_highlight_color),
		_ComponentsIndex(initial_components_index),
		_HighlightStrength(initial_highlight_strength)
	{

	}

};