#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/RandomNumberGenerator.h>
#include <Math/General/Vector.h>

namespace PathTracing
{

	namespace RayGeneration
	{

		/*
		*	Generates a uniform hemisphere sample oriented around the given normal..
		*/
		FORCE_INLINE Vector3<float32> RandomHemisphereUniform
		(
			const Vector3<float32> &normal,
			RandomNumberGenerator *const RESTRICT random_number_generator
		) NOEXCEPT
		{
			//Generate a point on a unit sphere - This will also be a unit direction vector.
			Vector3<float32> direction{ random_number_generator->RandomPointOnSphere() };

			//Flip the direction if it's not pointing the same way as the normal.
			if (Vector3<float32>::DotProduct(normal, direction) < 0.0f)
			{
				direction *= -1.0f;
			}

			//Return the direction.
			return direction;
		}

	}

}