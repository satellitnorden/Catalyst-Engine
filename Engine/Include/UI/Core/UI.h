#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace UI
{

	//Enumeration covering all anchors.
	enum class Anchor : uint8
	{
		CENTER,

		TOP,
		RIGHT,
		BOTTOM,
		LEFT,

		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_RIGHT,
		BOTTOM_LEFT
	};

	//Enumeration cover all horizontal alignments.
	enum class HorizontalAlignment : uint8
	{
		LEFT,
		CENTER,
		RIGHT
	};

	//Enumeration cover all vertical alignments.
	enum class VerticalAlignment : uint8
	{
		TOP,
		CENTER,
		BOTTOM
	};

	//Constants.
	namespace Constants
	{
		constexpr Vector2<float32> REFERENCE_RESOLUTION{ 1'920.0f, 1'080.0f };
	}

}