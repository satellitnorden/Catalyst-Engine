#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/StaticString.h>

//Math.
#include <Math/General/Vector.h>

namespace Audio
{

	/*
	*	Simple class holding piano roll information.
	*/
	class PianoRollInformation final
	{

	public:

		/*
		*	Key information class definition.
		*/
		class KeyInformation final
		{

		public:

			//The color.
			Vector3<float32> _Color{ -1.0f, -1.0f, -1.0f };

			//The tooltip.
			StaticString<64> _Tooltip{ "" };

		};

		//The key information.
		StaticArray<KeyInformation, 127> _KeyInformation;

	};

}