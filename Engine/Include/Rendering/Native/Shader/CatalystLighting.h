#if !defined(CATALYST_LIGHTING)
#define CATALYST_LIGHTING

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
#include "CatalystShaderCore.h"
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
#include "..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

CATALYST_SHADER_NAMESPACE_BEGIN(CatalystLighting)

	//Define constants.
	CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_MINIMUM_DOT_PRODUCT, 0.001f);

	//Forward declarations.
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateBidirectionalReflectanceDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleBidirectionalReflectanceDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateSpecularComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleSpecularComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateNormalDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleNormalDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateGeometry,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleGeometry,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateFresnel,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleFresnel,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateDiffuseComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleDiffuseComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateSpecularWeight,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleSpecularWeight,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_SIX_ARGUMENTS(	vec3,
													CalculateIndirectFresnel,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion);

	/*
	*	Calculates the attenuation based on the distance to a light source and it's radius.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_TWO_ARGUMENTS(float, CalculateAttenuation, float distance_to_light_source, float radius)
	{
		return 1.0f - CATALYST_SHADER_FUNCTION_MINIMUM(distance_to_light_source / radius, 1.0f);
	}

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
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateLighting,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Calculate the bidirectional reflectance distribution.
		vec3 bidirectional_reflectance_distribution = CalculateBidirectionalReflectanceDistribution(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		//Calculate the weakening factor.
		float irradiance_direction_normal_dot = CATALYST_SHADER_FUNCTION_DOT_PRODUCT(-irradiance_direction, normal);
		float weakening_factor = CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(irradiance_direction_normal_dot * 0.5f + 0.5f, CATALYST_SHADER_FUNCTION_MAXIMUM(irradiance_direction_normal_dot, 0.0f), thickness);

		//Calculate the lighting.
		return bidirectional_reflectance_distribution * irradiance * weakening_factor * ambient_occlusion;
	}

	/*
	*	Calculates the bidirectional reflectance distribution.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateBidirectionalReflectanceDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Calculate the specular component.
		vec3 specular_component = CalculateSpecularComponent(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		//Calculate the diffse component.
		vec3 diffuse_component = CalculateDiffuseComponent(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		//Calculate the specular weight	
		vec3 specular_weight = CalculateSpecularWeight(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		//Calculate the diffuse weight.
		vec3 diffuse_weight = (vec3(1.0f, 1.0f, 1.0f) - specular_weight) * (1.0f - metallic);

		//Calculate the bidirectional reflective distribution.
		return specular_component * specular_weight + diffuse_component * diffuse_weight;
	}

	/*
	*	Samples the bidirectional reflectance distribution.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleBidirectionalReflectanceDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Calculate the specular component.
		float specular_component = SampleSpecularComponent(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		//Calculate the diffse component.
		float diffuse_component = SampleDiffuseComponent(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		//Calculate the specular weight	
		float specular_weight = SampleSpecularWeight(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		//Calculate the diffuse weight.
		float diffuse_weight = (1.0f - specular_weight) * (1.0f - metallic);

		//Calculate the bidirectional reflective distribution.
		return specular_component * specular_weight + diffuse_component * diffuse_weight;
	}

	/*
	*	Calculates and returns the specular component.
	*
	*	Calculated using the Cook-Torrance algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateSpecularComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_FLOAT_EPSILON, 1.192092896e-07F);

		//Calculate the numerator.
		vec3 normal_distribution = CalculateNormalDistribution(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);
		vec3 geometry = CalculateGeometry(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);
		vec3 fresnel = CalculateFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		vec3 numerator = normal_distribution * geometry * fresnel;

		//Calculate the denominator.
		float outgoing_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(outgoing_direction, normal), 0.0f);
		float irradiance_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, -irradiance_direction), 0.0f);

		float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(4.0f * outgoing_direction_coefficient * irradiance_direction_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

		//Calculate the specular component.
		return vec3(numerator / denominator);
	}

	/*
	*	Samples and returns the specular component.
	*
	*	Calculated using the Cook-Torrance algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleSpecularComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_FLOAT_EPSILON, 1.192092896e-07F);

		//Calculate the numerator.
		float normal_distribution = SampleNormalDistribution(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);
		float geometry = SampleGeometry(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);
		float fresnel = SampleFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);

		float numerator = normal_distribution * geometry * fresnel;

		//Calculate the denominator.
		float outgoing_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(outgoing_direction, normal), 0.0f);
		float irradiance_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, -irradiance_direction), 0.0f);

		float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(4.0f * outgoing_direction_coefficient * irradiance_direction_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

		//Calculate the specular component.
		return numerator / denominator;
	}

	/*
	*	Calculates and returns the normal distribution.
	*
	*	Calculated using the Trowbridge-Reitz GGX algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateNormalDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_FLOAT_EPSILON, 1.192092896e-07F);
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_PI, 3.141592f);

		//Calculate the halfway vector.
		vec3 halfway_vector = CATALYST_SHADER_FUNCTION_NORMALIZE(outgoing_direction + -irradiance_direction);

		//Calculate the roughness squared.
		float roughness_squared = roughness * roughness;

		//Calculate the numerator.
		float numerator = roughness_squared;

		//Calculate the denominator.
		float halfway_vector_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, halfway_vector), 0.0f);
		float halfway_vector_coefficient_squared = halfway_vector_coefficient * halfway_vector_coefficient;
		float denominator_coefficient = halfway_vector_coefficient_squared * (roughness_squared - 1.0f) + 1.0f;

		float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_LIGHTING_PI * denominator_coefficient * denominator_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

		//Calculate the normal distribution.
		return vec3(numerator / denominator);
	}

	/*
	*	Samples and returns the normal distribution.
	*
	*	Calculated using the Trowbridge-Reitz GGX algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleNormalDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_FLOAT_EPSILON, 1.192092896e-07F);
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_PI, 3.141592f);

		//Calculate the halfway vector.
		vec3 halfway_vector = CATALYST_SHADER_FUNCTION_NORMALIZE(outgoing_direction + -irradiance_direction);

		//Calculate the roughness squared.
		float roughness_squared = roughness * roughness;

		//Calculate the numerator.
		float numerator = roughness_squared;

		//Calculate the denominator.
		float halfway_vector_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, halfway_vector), 0.0f);
		float halfway_vector_coefficient_squared = halfway_vector_coefficient * halfway_vector_coefficient;
		float denominator_coefficient = halfway_vector_coefficient_squared * (roughness_squared - 1.0f) + 1.0f;

		float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_LIGHTING_PI * denominator_coefficient * denominator_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

		//Calculate the normal distribution.
		return numerator / denominator;
	}

	/*
	*	Calculates and returns the geometry.
	*
	*	Calculated using the Schlick-GGX and Smith algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateGeometry,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Calculate the outgoing direction coefficient.
		float outgoing_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, outgoing_direction), 0.0f);

		//Calculate the irradiance direction coefficient.
		float irradiance_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, -irradiance_direction), 0.0f);

		//Calculate the roughness coefficient.
		float roughness_coefficient = ((roughness + 1.0f) * (roughness + 1.0f)) / 8.0f;

		//Calculate the first coefficient.
		float first_coefficient;

		{
			//Calculate the numerator.
			float numerator = outgoing_direction_coefficient;

			//Calculate the denominator.
			float denominator = outgoing_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

			first_coefficient = numerator / denominator;
		}

		//Calculate the second coefficient.
		float second_coefficient;

		{
			//Calculate the numerator.
			float numerator = irradiance_direction_coefficient;

			//Calculate the denominator.
			float denominator = irradiance_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

			second_coefficient = numerator / denominator;
		}

		//Calculate the geometry.
		return vec3(first_coefficient * second_coefficient);
	}

	/*
	*	Samples and returns the geometry.
	*
	*	Calculated using the Schlick-GGX and Smith algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleGeometry,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Calculate the outgoing direction coefficient.
		float outgoing_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, outgoing_direction), 0.0f);

		//Calculate the irradiance direction coefficient.
		float irradiance_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, -irradiance_direction), 0.0f);

		//Calculate the roughness coefficient.
		float roughness_coefficient = ((roughness + 1.0f) * (roughness + 1.0f)) / 8.0f;

		//Calculate the first coefficient.
		float first_coefficient;

		{
			//Calculate the numerator.
			float numerator = outgoing_direction_coefficient;

			//Calculate the denominator.
			float denominator = outgoing_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

			first_coefficient = numerator / denominator;
		}

		//Calculate the second coefficient.
		float second_coefficient;

		{
			//Calculate the numerator.
			float numerator = irradiance_direction_coefficient;

			//Calculate the denominator.
			float denominator = irradiance_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

			second_coefficient = numerator / denominator;
		}

		//Calculate the geometry.
		return first_coefficient * second_coefficient;
	}

	/*
	*	Calculates and returns the fresnel.
	*	
	*	Calculated using the cook-torrance algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateFresnel,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Calculate the surface color.
		vec3 surface_color = CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(vec3(0.04f, 0.04f, 0.04f), albedo, metallic);

		//Calculate the halfway vector.
		vec3 halfway_vector = CATALYST_SHADER_FUNCTION_NORMALIZE(outgoing_direction + -irradiance_direction);

		//Calculate the halfway vector coefficient.
		float halfway_vector_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(halfway_vector, outgoing_direction), 0.0f);
		float halfway_vector_minus_one = 1.0f - halfway_vector_coefficient;

		//Calculate the fresnel coefficient.
		float fresnel_coefficient = halfway_vector_minus_one * halfway_vector_minus_one * halfway_vector_minus_one * halfway_vector_minus_one * halfway_vector_minus_one;

		//Calculate the fresnel.
		return surface_color + (vec3(1.0f, 1.0f, 1.0f) - surface_color) * fresnel_coefficient;
	}

	/*
	*	Samples and returns the fresnel.
	*	
	*	Calculated using the cook-torrance algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleFresnel,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Calculate the surface scalar.
		float surface_scalar = CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(0.04f, 1.0f, metallic);

		//Calculate the halfway vector.
		vec3 halfway_vector = CATALYST_SHADER_FUNCTION_NORMALIZE(outgoing_direction + -irradiance_direction);

		//Calculate the halfway vector coefficient.
		float halfway_vector_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(halfway_vector, outgoing_direction), 0.0f);
		float halfway_vector_minus_one = 1.0f - halfway_vector_coefficient;

		//Calculate the fresnel coefficient.
		float fresnel_coefficient = halfway_vector_minus_one * halfway_vector_minus_one * halfway_vector_minus_one * halfway_vector_minus_one * halfway_vector_minus_one;

		//Calculate the fresnel.
		return surface_scalar + (1.0f, 1.0f, 1.0f - surface_scalar) * fresnel_coefficient;
	}

	/*
	*	Calculates and returns the diffuse component.
	*
	*	Calculated using the Lambertian Diffuse algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateDiffuseComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_INVERSE_PI, 0.318309f);

		//Calculate the diffuse component.
		return albedo * CATALYST_LIGHTING_INVERSE_PI;
	}

	/*
	*	Samples and returns the diffuse component.
	*
	*	Calculated using the Lambertian Diffuse algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleDiffuseComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_INVERSE_PI, 0.318309f);

		//Calculate the diffuse component.
		return CATALYST_LIGHTING_INVERSE_PI;
	}

	/*
	*	Calculates and returns the specular weight.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateSpecularWeight,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//The fresnel is directly related to the specular weight, so use that.
		return CalculateFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);
	}

	/*
	*	Samples and returns the specular weight.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	float,
													SampleSpecularWeight,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//The fresnel is directly related to the specular weight, so use that.
		return SampleFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, thickness, irradiance_direction, irradiance);
	}

	/*
	*	Calculates indirect lighting. Returns the radiance transmitted from the surface in the outgoing direction.
	*
	*	Arguments;
	*	
	*	- outgoing_direction: A direction vector from the point on the surface being shaded in the outgoing direction.
	*	- albedo: The albedo of the surface point being shaded.
	*	- normal: The normal of the surface point being shaded.
	*	- roughness: The roughness of the surface point being shaded.
	*	- metallic: THe metallic of the surface point being shaded.
	*	- ambient_occlusion: The ambient occlusion of the surface point being shaded.
	*	- diffuse_irradiance: The incoming diffuse irradiance towards the surface point being shaded.
	*	- specular_irradiance: The incoming specular irradiance towards the surface point being shaded.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateIndirectLighting,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 diffuse_irradiance,
													vec3 specular_irradiance,
													vec2 specular_bias)
	{
		//Calculate the specular weight.
		vec3 specular_weight = CalculateIndirectFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion);

		//Calculate the diffuse weight.
		vec3 diffuse_weight = vec3(1.0f, 1.0f, 1.0f) - specular_weight * (1.0f - metallic);

		//Calculate the diffuse component.
		vec3 diffuse_component = diffuse_irradiance * albedo;

		//Calculate the specular component 
		vec3 specular_component = specular_irradiance * (specular_weight * specular_bias.x + specular_bias.y);

		//Calculate the lighting.
		return (diffuse_component * diffuse_weight + specular_component) * ambient_occlusion;
	}

	/*
	*	Calculates and returns the indirect fresnel.
	*	
	*	Calculated using the cook-torrance algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SIX_ARGUMENTS(	vec3,
													CalculateIndirectFresnel,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion)
	{
		//Calculate the surface color.
		vec3 surface_color = CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(vec3(0.04f, 0.04f, 0.04f), albedo, metallic);

		//Calculate cos theta.
		float cos_theta = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, outgoing_direction), 0.0f);

		//Calculate the fresnel.
		return surface_color + (CATALYST_SHADER_FUNCTION_MAXIMUM(vec3(1.0f - roughness), surface_color) - surface_color) * CATALYST_SHADER_FUNCTION_POWER(CATALYST_SHADER_FUNCTION_CLAMP(1.0f - cos_theta, 0.0f, 1.0f), 5.0f);
	}

	/*
	*	Calculates simplified lighting, based on the Blinn-Phong model. Returns the radiance transmitted from the surface in the outgoing direction.
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
	CATALYST_SHADER_FUNCTION_RETURN_NINE_ARGUMENTS(	vec3,
													CalculateSimplifiedLighting,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													float thickness,
													vec3 irradiance_direction,
													vec3 irradiance)
	{
		//Calculate diffuse.
		float diffuse_factor = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, -irradiance_direction), 0.0f);
		vec3 diffuse = diffuse_factor * irradiance * (albedo * (1.0f - metallic));

		//Calculate specular.
		vec3 halfway_direction = CATALYST_SHADER_FUNCTION_NORMALIZE((-irradiance_direction) + outgoing_direction);
		float specular_power = CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(256.0f, 1.0f, roughness * (1.0f - metallic));
		float specular_factor = CATALYST_SHADER_FUNCTION_POWER(CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, halfway_direction), 0.0f), specular_power);
		vec3 specular = specular_factor * irradiance * albedo;

		//Add them together!
		return (diffuse + specular) * ambient_occlusion;
	}

CATALYST_SHADER_NAMESPACE_END()
#endif