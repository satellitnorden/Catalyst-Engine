#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace PathTracing
{

	/*
	*	Class holding data about a hit surface.
	*/
	class SurfaceInformation final
	{

	public:

		//The hit distance.
		float32 _HitDistance{ FLOAT32_MAXIMUM };

		//The position.
		Vector3<float32> _Position;

		//The normal.
		Vector3<float32> _Normal;

	};
}