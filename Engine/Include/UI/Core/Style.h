#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace UI
{

	/*
	*	UI Style class, containing all of the different parameters for a given style.
	*/
	class Style final
	{

	public:

		//The unchecked idle color.
		Vector4<float32> _UncheckedIdleColor;

		//The unchecked hovered color.
		Vector4<float32> _UncheckedHoveredColor;

		//The unchecked pressed color.
		Vector4<float32> _UncheckedPressedColor;

		//The checked idle color.
		Vector4<float32> _CheckedIdleColor;

		//The checked hovered color.
		Vector4<float32> _CheckedHoveredColor;

		//The checked pressed color.
		Vector4<float32> _CheckedPressedColor;

		//The rounding.
		float32 _Rounding;

	};

}