#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Lighting.
#include <Lighting/PhysicallyBasedLighting.h>

//Math.
#include <Math/General/Vector.h>

namespace PathTracing
{

	class BRDF final
	{

	public:

		/*
		*	The samples this BRDF.
		*
		*	- outgoing_direction: A direction vector from the point on the surface being shaded in the outgoing direction.
		*	- albedo: The albedo of the surface point being shaded.
		*	- normal: The normal of the surface point being shaded.
		*	- roughness: The roughness of the surface point being shaded.
		*	- metallic: The metallic of the surface point being shaded.
		*	- radiance_direction: A direction vector going from the entity emitting irradiance toward the surface point being shaded.
		*/
		FORCE_INLINE static NO_DISCARD Vector3<float32> Sample
		(
			const Vector3<float32> &outgoing_direction,
			const Vector3<float32> &albedo,
			const Vector3<float32> &normal,
			const float32 roughness,
			const float32 metallic,
			const Vector3<float32> &radiance_direction
		) NOEXCEPT
		{
			return PhysicallyBasedLighting::BidirectionalReflectanceDistribution
			(
				outgoing_direction,
				albedo,
				normal,
				roughness,
				metallic,
				1.0f,
				radiance_direction
			);
		}
	};
}