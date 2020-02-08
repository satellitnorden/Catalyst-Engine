#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector.h>

class CatalystLighting final
{

public:

	/*
	*	Calculates lighting. Returns the radiance transmitted from the surface in the outgoing direction.
	*
	*	Arguments;
	*	
	*	- outgoing_direction: A direction vector from the point on the surface being shaded in the outgoing direction.
	*	- albedo: The albedo of the surface point being shaded.
	*	- normal: The normal of the surface point being shaded.
	*	- roughness: The roughness of the surface point being shaded.
	*	- metallic: THe metallic of the surface point being shaded.
	*	- ambient_occlusion: The ambient occlusion of the surface point being shaded.
	*	- thickness: The thickness of the surface point being shaded.
	*	- irradiance_direction: A direction vector going from the entity emitting irradiance toward the surface point being shaded.
	*	- irradiance: The incoming irradiance towards the surface point being shaded.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateLighting(const Vector3<float> &outgoing_direction,
																	const Vector3<float> &albedo,
																	const Vector3<float> &normal,
																	const float roughness,
																	const float metallic,
																	const float ambient_occlusion,
																	const float thickness,
																	const Vector3<float> &irradiance_direction,
																	const Vector3<float> &irradiance) NOEXCEPT
	{
		//Calculate the bidirectional reflectance distribution.
		const Vector3<float> bidirectional_reflectance_distribution{ CalculateBidirectionalReflectanceDistribution(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance) };

		//Calculate the weakening factor.
		const float weakening_factor{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(-irradiance_direction, normal), 0.0f) };

		//Calculate the lighting.
		return bidirectional_reflectance_distribution * irradiance * weakening_factor;
	}

private:

	/*
	*	Calculates the bidirectional reflectance distribution.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateBidirectionalReflectanceDistribution(const Vector3<float>& outgoing_direction,
																								const Vector3<float>& albedo,
																								const Vector3<float>& normal,
																								const float roughness,
																								const float metallic,
																								const float ambient_occlusion,
																								const float thickness,
																								const Vector3<float>& irradiance_direction,
																								const Vector3<float>& irradiance) NOEXCEPT
	{
		//Calculate the specular component.
		const Vector3<float> specular_component{ CalculateSpecularComponent(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance) };

		//Calculate the diffse component.
		const Vector3<float> diffuse_component{ CalculateDiffuseComponent(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance) };

		//Calculate the specular weight	
		const Vector3<float> specular_weight{ CalculateSpecularWeight(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance) };

		//Calculate the diffuse weight.
		const Vector3<float> diffuse_weight{ (Vector3<float>(1.0f, 1.0f, 1.0f) - specular_weight) * (1.0f - metallic) };

		//Calculate the bidirectional reflective distribution.
		return specular_component * specular_weight + diffuse_component * diffuse_weight;
	}

	/*
	*	Calculates and returns the specular component.
	*
	*	Calculated using the Cook-Torrance algorithm.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateSpecularComponent(	const Vector3<float>& outgoing_direction,
																				const Vector3<float>& albedo,
																				const Vector3<float>& normal,
																				const float roughness,
																				const float metallic,
																				const float ambient_occlusion,
																				const float thickness,
																				const Vector3<float>& irradiance_direction,
																				const Vector3<float>& irradiance) NOEXCEPT
	{
		//Calculate the numerator.
		const Vector3<float> normal_distribution{ CalculateNormalDistribution(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance) };
		const Vector3<float> geometry{ CalculateGeometry(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance) };
		const Vector3<float> fresnel{ CalculateFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance) };

		const Vector3<float> numerator{ normal_distribution * geometry * fresnel };

		//Calculate the denominator.
		const float outgoing_direction_coefficient{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(outgoing_direction, normal), 0.0f) };
		const float irradiance_direction_coefficient{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(normal, -irradiance_direction), 0.0f) };

		const float denominator{ CatalystBaseMath::Maximum<float>(4.0f * outgoing_direction_coefficient * irradiance_direction_coefficient, FLOAT_EPSILON) };

		//Calculate the specular component.
		return Vector3<float>(numerator / denominator);
	}

	/*
	*	Calculates and returns the normal distribution.
	*
	*	Calculated using the Trowbridge-Reitz GGX algorithm.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateNormalDistribution(	const Vector3<float>& outgoing_direction,
																				const Vector3<float>& albedo,
																				const Vector3<float>& normal,
																				const float roughness,
																				const float metallic,
																				const float ambient_occlusion,
																				const float thickness,
																				const Vector3<float>& irradiance_direction,
																				const Vector3<float>& irradiance) NOEXCEPT
	{
		//Calculate the halfway vector.
		const Vector3<float> halfway_vector{ Vector3<float>::Normalize(outgoing_direction + -irradiance_direction) };

		//Calculate the roughness squared.
		const float roughness_squared{ roughness * roughness };

		//Calculate the numerator.
		const float numerator{ roughness_squared };

		//Calculate the denominator.
		const float halfway_vector_coefficient{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(normal, halfway_vector), 0.0f) };
		const float halfway_vector_coefficient_squared{ halfway_vector_coefficient * halfway_vector_coefficient };
		const float denominator_coefficient{ halfway_vector_coefficient_squared * (roughness_squared - 1.0f) + 1.0f };

		const float denominator{ CatalystBaseMathConstants::PI * denominator_coefficient * denominator_coefficient };

		//Calculate the normal distribution.
		return Vector3<float>(numerator / denominator);
	}

	/*
	*	Calculates and returns the geometry.
	*
	*	Calculated using the Schlick-GGX and Smith algorithm.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateGeometry(const Vector3<float>& outgoing_direction,
																	const Vector3<float>& albedo,
																	const Vector3<float>& normal,
																	const float roughness,
																	const float metallic,
																	const float ambient_occlusion,
																	const float thickness,
																	const Vector3<float>& irradiance_direction,
																	const Vector3<float>& irradiance) NOEXCEPT
	{
		//Calculate the outgoing direction coefficient.
		const float outgoing_direction_coefficient{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(normal, outgoing_direction), 0.0f) };

		//Calculate the irradiance direction coefficient.
		const float irradiance_direction_coefficient{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(normal, -irradiance_direction), 0.0f) };

		//Calculate the roughness coefficient.
		const float roughness_coefficient{ ((roughness + 1.0f) * (roughness + 1.0f)) / 8.0f };

		//Calculate the first coefficient.
		float first_coefficient;

		{
			//Calculate the numerator.
			const float numerator{ outgoing_direction_coefficient };

			//Calculate the denominator.
			const float denominator{ CatalystBaseMath::Maximum<float>(outgoing_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient, FLOAT_EPSILON) };

			first_coefficient = numerator / denominator;
		}

		//Calculate the second coefficient.
		float second_coefficient;

		{
			//Calculate the numerator.
			const float numerator{ irradiance_direction_coefficient };

			//Calculate the denominator.
			const float denominator{ CatalystBaseMath::Maximum<float>(irradiance_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient, FLOAT_EPSILON) };

			second_coefficient = numerator / denominator;
		}

		//Calculate the geometry.
		return Vector3<float>(first_coefficient * second_coefficient);
	}

	/*
	*	Calculates and returns the fresnel.
	*	
	*	Calculated using the cook-torrance algorithm.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateFresnel(	const Vector3<float>& outgoing_direction,
																	const Vector3<float>& albedo,
																	const Vector3<float>& normal,
																	const float roughness,
																	const float metallic,
																	const float ambient_occlusion,
																	const float thickness,
																	const Vector3<float>& irradiance_direction,
																	const Vector3<float>& irradiance) NOEXCEPT
	{
		//Calculate the surface color.
		const Vector3<float> surface_color{ CatalystBaseMath::LinearlyInterpolate<Vector3<float>>(Vector3<float>(0.04f, 0.04f, 0.04f), albedo, metallic) };

		//Calculate the halfway vector.
		const Vector3<float> halfway_vector{ Vector3<float>::Normalize(outgoing_direction + -irradiance_direction) };

		//Calculate the halfway vector coefficient.
		const float halfway_vector_coefficient{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(halfway_vector, outgoing_direction), 0.0f) };

		//Calculate the fresnel coefficient.
		const float fresnel_coefficient{ (1.0f - halfway_vector_coefficient) * (1.0f - halfway_vector_coefficient) * (1.0f - halfway_vector_coefficient) * (1.0f - halfway_vector_coefficient) * (1.0f - halfway_vector_coefficient) };

		//Calculate the fresnel.
		return surface_color + (Vector3<float>(1.0f, 1.0f, 1.0f) - surface_color) * fresnel_coefficient;
	}

	/*
	*	Calculates and returns the diffuse component.
	*
	*	Calculated using the Lambertian Diffuse algorithm.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateDiffuseComponent(const Vector3<float>& outgoing_direction,
																			const Vector3<float>& albedo,
																			const Vector3<float>& normal,
																			const float roughness,
																			const float metallic,
																			const float ambient_occlusion,
																			const float thickness,
																			const Vector3<float>& irradiance_direction,
																			const Vector3<float>& irradiance) NOEXCEPT
	{
		//Calculate the diffuse component.
		return albedo * CatalystBaseMathConstants::INVERSE_PI;
	}

	/*
	*	Calculates and returns the specular weight.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> CalculateSpecularWeight(	const Vector3<float>& outgoing_direction,
																			const Vector3<float>& albedo,
																			const Vector3<float>& normal,
																			const float roughness,
																			const float metallic,
																			const float ambient_occlusion,
																			const float thickness,
																			const Vector3<float>& irradiance_direction,
																			const Vector3<float>& irradiance) NOEXCEPT
	{
		//The fresnel is directly related to the specular weight, so use that.
		return CalculateFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);
	}

};