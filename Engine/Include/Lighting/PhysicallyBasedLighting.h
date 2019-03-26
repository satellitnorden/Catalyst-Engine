#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace PhysicallyBasedLighting
{

	/*
	*   Calculates a light.
	*
	*   viewDirection - Direction vector going from the surface point to the perceiving point.
	*   lightDirection - Direction vector going from the surface point to the light point.
	*   normal - The normal vector of the surface point.
	*   roughness - The roughness of the surface at the surface point.
	*   metallic - The metallic of the surface at the surface point.
	*   albedo - The albedo of the surface at the surface point.
	*   radiance - The radiance of the light.
	*/
	NO_DISCARD Vector3<float> CalculateLight(	const Vector3<float> &viewDirection,
												const Vector3<float> &lightDirection,
												const Vector3<float> &normal,
												const float roughness,
												const float metallic,
												const Vector3<float> &albedo,
												const Vector3<float> &radiance) NOEXCEPT;

}