//Header file.
#include <Lighting/PhysicallyBasedLighting.h>

namespace PhysicallyBasedLightingInternal
{

	/*
	*   Calculates the distribution.
	*/
	float CalculateDistribution(float roughness,
		Vector3<float> normalDirection,
		Vector3<float> halfwayDirection)
	{
		float roughnessSquared = roughness * roughness;
		float roughnessSquaredSquared = roughnessSquared * roughnessSquared;
		float normalHalfwayAngle = CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(normalDirection, halfwayDirection), 0.0f);
		float normalHalfwayAngleSquared = normalHalfwayAngle * normalHalfwayAngle;

		float denominator = normalHalfwayAngleSquared * (roughnessSquaredSquared - 1.0f) + 1.0f;

		return roughnessSquaredSquared / (CatalystBaseMathConstants::PI * denominator * denominator);
	}

	/*
	*   Calculates the fresnel.
	*/
	Vector3<float> CalculateFresnel(Vector3<float> surfaceColor,
		float lightViewAngle)
	{
		return surfaceColor + (surfaceColor - 1.0f) * CatalystBaseMath::PowerOf(CatalystBaseMath::Maximum<float>(1.0f - lightViewAngle, 0.0f), 5);
	}

	/*
	*   Calculates the fresnel with roughness in mind.
	*/
	Vector3<float> CalculateFresnelRoughness(Vector3<float> surfaceColor,
		float roughness,
		float lightViewAngle)
	{
		return surfaceColor + (Vector3<float>::Maximum(Vector3<float>(1.0f - roughness), surfaceColor) - surfaceColor) * CatalystBaseMath::PowerOf(CatalystBaseMath::Maximum<float>(1.0f - lightViewAngle, 0.0f), 5);
	}

	/*
	*   Calculates the geometry.
	*/
	float CalculateGeometry(float roughness,
		float lightAngle,
		float viewAngle)
	{
		float geometryRoughness = roughness + 1.0f;
		geometryRoughness = (geometryRoughness * geometryRoughness) / 8.0f;

		float lightObstruction = lightAngle / (lightAngle * (1.0f - geometryRoughness) + geometryRoughness);
		float viewObstruction = viewAngle / (viewAngle * (1.0f - geometryRoughness) + geometryRoughness);

		return lightObstruction * viewObstruction;
	}

	/*
	*   Calculates the surface color of a fragment.
	*/
	Vector3<float> CalculateSurfaceColor(Vector3<float> albedo,
		float metallic)
	{
		return Vector3<float>::LinearlyInterpolate(Vector3<float>(0.04f), albedo, metallic);
	}

}

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
												const Vector3<float> &radiance) NOEXCEPT
	{
		Vector3<float> halfwayDirection = Vector3<float>::Normalize(viewDirection + lightDirection);
		float lightViewAngle = CatalystBaseMath::Clamp<float>(Vector3<float>::DotProduct(halfwayDirection, viewDirection), 0.0f, 1.0f);
		float lightAngle = CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(normal, lightDirection), 0.0f);
		float viewAngle = CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(normal, viewDirection), 0.0f);

		float distribution = PhysicallyBasedLightingInternal::CalculateDistribution(roughness, normal, halfwayDirection);
		float geometry = PhysicallyBasedLightingInternal::CalculateGeometry(roughness, lightAngle, viewAngle);
		Vector3<float> fresnel = PhysicallyBasedLightingInternal::CalculateFresnel(PhysicallyBasedLightingInternal::CalculateSurfaceColor(albedo, metallic), lightViewAngle);

		Vector3<float> diffuseComponent = Vector3<float>(1.0f) - fresnel;
		diffuseComponent *= 1.0f - metallic;

		Vector3<float> nominator = distribution * geometry * fresnel;
		float denominator = 4.0f * viewAngle * lightAngle;
		Vector3<float> specularComponent = denominator != 0.0f ? nominator / denominator : Vector3<float>(0.0f);

		//Return the combined components.
		return (diffuseComponent * albedo / CatalystBaseMathConstants::PI + specularComponent) * radiance * lightAngle;
	}

}