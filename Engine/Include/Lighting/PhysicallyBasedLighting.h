#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector.h>

namespace PhysicallyBasedLighting
{

	////////////
	// COMMON //
	////////////

	/*
	*	The distribution function.
	*	This approximates the amount the surface's microfacets are aligned to the halfway vector,
	*	influenced by the roughness of the surface; this is the primary function approximating the microfacets.
	*	Trowbridge-Reitz GGX.
	*/
	float Distribution(float roughness, float microsurface_angle)
	{
		float roughness_squared = pow(roughness, 4.0f);
		float microsurface_angle_squared = microsurface_angle * microsurface_angle;

		float nominator = roughness_squared;
		float denominator = microsurface_angle_squared * (roughness_squared - 1.0f) + 1.0f;
		denominator = CatalystBaseMathConstants::PI * denominator * denominator;

		return nominator / denominator;
	}

	/*
	*	The geometry function.
	*	This describes the self-shadowing property of the microfacets.
	*	When a surface is relatively rough, the surface's microfacets can overshadow other microfacets reducing the light the surface reflects.
	*	Schlick-GGX.
	*/
	float Geometry(Vector3<float32> normal, Vector3<float32> outgoing_direction, Vector3<float32> radiance_direction, float roughness)
	{
		//Calculate the outgoing direction coefficient.
		float outgoing_direction_coefficient = CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(normal, outgoing_direction), 0.0f);

		//Calculate the irradiance direction coefficient.
		float irradiance_direction_coefficient = CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(normal, -radiance_direction), 0.0f);

		//Calculate the roughness coefficient.
		float roughness_coefficient = roughness + 1.0f;
		roughness_coefficient = (roughness_coefficient * roughness_coefficient) / 8.0f;

		//Calculate the first coefficient.
		float first_coefficient;

		{
			//Calculate the nominator.
			float nominator = outgoing_direction_coefficient;

			//Calculate the denominator.
			float denominator = outgoing_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

			first_coefficient = nominator / denominator;
		}

		//Calculate the second coefficient.
		float second_coefficient;

		{
			//Calculate the nominator.
			float nominator = irradiance_direction_coefficient;

			//Calculate the denominator.
			float denominator = irradiance_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

			second_coefficient = nominator / denominator;
		}

		//Calculate the geometry.
		return first_coefficient * second_coefficient;
	}

	/*
	*	The fresnel function.
	*	The Fresnel equation describes the ratio of surface reflection at different surface angles.
	*/
	Vector3<float32> Fresnel(Vector3<float32> surface_color, float difference_angle)
	{
		//Calculate the fresnel.
		return surface_color + (Vector3<float32>(1.0f) - surface_color) * pow(1.0f - difference_angle, 5.0f);
	}

	/*
	*	The lambert diffuse function.
	*/
	Vector3<float32> LambertDiffuse(Vector3<float32> albedo)
	{
		return albedo / CatalystBaseMathConstants::PI;
	}

	/*
	*	The disney diffuse function.
	*	Inspired by: https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf
	*/
	Vector3<float32> DisneyDiffuse(Vector3<float32> albedo, float roughness, float difference_angle, float radiance_angle, float outgoing_angle)
	{
		//Calculate some stuff.
		float FD90 = 0.5f + 2.0f * roughness * pow(cos(difference_angle), 2.0f);

		//Set up terms.
		Vector3<float32> term_1 = albedo / CatalystBaseMathConstants::PI;
		float term_2 = 1.0f + (FD90 - 1.0f) * pow(1.0f - cos(radiance_angle), 5.0f);
		float term_3 = 1.0f + (FD90 - 1.0f) * pow(1.0f - cos(outgoing_angle), 5.0f);

		return term_1 * term_2 * term_3;
	}

	//////////////
	// LIGHTING //
	//////////////
	/*
	*	Calculates lighting. Returns the radiance transmitted from the surface in the outgoing direction.
	*
	*	Arguments;
	*
	*	- outgoing_direction: A direction vector from the point on the surface being shaded in the outgoing direction.
	*	- albedo: The albedo of the surface point being shaded.
	*	- normal: The normal of the surface point being shaded.
	*	- roughness: The roughness of the surface point being shaded.
	*	- metallic: The metallic of the surface point being shaded.
	*	- ambient_occlusion: The ambient occlusion of the surface point being shaded.
	*	- thickness: The thickness of the surface point being shaded.
	*	- radiance_direction: A direction vector going from the entity emitting irradiance toward the surface point being shaded.
	*	- radiance: The incoming irradiance towards the surface point being shaded.
	*/
	Vector3<float32> CalculateLighting
	(
		Vector3<float32> outgoing_direction,
		Vector3<float32> albedo,
		Vector3<float32> normal,
		float roughness,
		float metallic,
		float ambient_occlusion,
		float thickness,
		Vector3<float32> radiance_direction,
		Vector3<float32> radiance
	)
	{
		//Calculate the microsurface normal.
		Vector3<float32> microsurface_normal = Vector3<float32>::Normalize(outgoing_direction + -radiance_direction);

		//Calculate the surface color.
		Vector3<float32> surface_color = CatalystBaseMath::LinearlyInterpolate(Vector3<float32>(0.04f), albedo, metallic);

		//Calculate the angle values.
		float outgoing_angle = CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(normal, outgoing_direction), 0.0f);
		float radiance_angle = CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(normal, -radiance_direction), 0.0f);
		float microsurface_angle = CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(normal, microsurface_normal), 0.0f);
		float difference_angle = CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(-radiance_direction, microsurface_normal), 0.0f);

		//Calculate the normal distribution.
		float distribution = Distribution(roughness, microsurface_angle);

		//Calculate the geometry.
		float geometry = Geometry(normal, outgoing_direction, radiance_direction, roughness);

		//Calculate the fresnel.
		Vector3<float32> fresnel = Fresnel(surface_color, difference_angle);

		//Calculate the diffuse component.
		Vector3<float32> diffuse_component;

		{
			diffuse_component = DisneyDiffuse(albedo, roughness, difference_angle, radiance_angle, outgoing_angle);

			diffuse_component *= (Vector3<float32>(1.0f) - fresnel) * (1.0f - metallic);
		}

		//Calculate the specular component.
		Vector3<float32> specular_component;

		{
			Vector3<float32> nominator = Vector3<float32>(distribution) * Vector3<float32>(geometry) * fresnel;
			float denominator = CatalystBaseMath::Maximum<float32>(4.0f * outgoing_angle * radiance_angle, 0.00001f);

			specular_component = nominator / denominator;
		}

		//Calculate the weakening factor.
		float weakening_factor = Vector3<float32>::DotProduct(normal, -radiance_direction);
		weakening_factor = CatalystBaseMath::LinearlyInterpolate(weakening_factor * 0.5f + 0.5f, CatalystBaseMath::Maximum<float32>(weakening_factor, 0.0f), thickness);

		return (diffuse_component + specular_component) * radiance * weakening_factor;
	}

}