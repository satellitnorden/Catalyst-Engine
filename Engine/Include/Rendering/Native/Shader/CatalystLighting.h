#if !defined(CATALYST_LIGHTING)
#define CATALYST_LIGHTING

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
#include "CatalystShaderCore.h"
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
#include "..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

CATALYST_SHADER_NAMESPACE_BEGIN(CatalystLighting)

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

	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectBidirectionalReflectanceDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectSpecularComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectNormalDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectGeometry,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectFresnel,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectDiffuseComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance);

	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectSpecularWeight,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance);

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
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_FLOAT_EPSILON, 1.192092896e-07F);

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
			float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(outgoing_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

			first_coefficient = numerator / denominator;
		}

		//Calculate the second coefficient.
		float second_coefficient;

		{
			//Calculate the numerator.
			float numerator = irradiance_direction_coefficient;

			//Calculate the denominator.
			float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(irradiance_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

			second_coefficient = numerator / denominator;
		}

		//Calculate the geometry.
		return vec3(first_coefficient * second_coefficient);
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

		//Calculate the fresnel coefficient.
		float fresnel_coefficient = (1.0f - halfway_vector_coefficient) * (1.0f - halfway_vector_coefficient) * (1.0f - halfway_vector_coefficient) * (1.0f - halfway_vector_coefficient) * (1.0f - halfway_vector_coefficient);

		//Calculate the fresnel.
		return surface_color + (vec3(1.0f, 1.0f, 1.0f) - surface_color) * fresnel_coefficient;
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
	*	- irradiance: The incoming irradiance towards the surface point being shaded.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectLighting,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance)
	{
		//Calculate the bidirectional reflectance distribution.
		vec3 bidirectional_reflectance_distribution = CalculateIndirectBidirectionalReflectanceDistribution(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, irradiance);

		//Calculate the lighting.
		return bidirectional_reflectance_distribution * irradiance * ambient_occlusion;
	}

	/*
	*	Calculates the indirect bidirectional reflectance distribution.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectBidirectionalReflectanceDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance)
	{
		//Calculate the specular component.
		vec3 specular_component = CalculateIndirectSpecularComponent(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, irradiance);

		//Calculate the diffse component.
		vec3 diffuse_component = CalculateIndirectDiffuseComponent(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, irradiance);

		//Calculate the specular weight	
		vec3 specular_weight = CalculateIndirectSpecularWeight(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, irradiance);

		//Calculate the diffuse weight.
		vec3 diffuse_weight = (vec3(1.0f, 1.0f, 1.0f) - specular_weight) * (1.0f - metallic);

		//Calculate the bidirectional reflective distribution.
		return specular_component * specular_weight + diffuse_component * diffuse_weight;
	}

	/*
	*	Calculates and returns the indirect specular component.
	*
	*	Calculated using the Cook-Torrance algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectSpecularComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_FLOAT_EPSILON, 1.192092896e-07F);

		//Calculate the numerator.
		vec3 normal_distribution = CalculateIndirectNormalDistribution(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, irradiance);
		vec3 geometry = CalculateIndirectGeometry(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, irradiance);
		vec3 fresnel = CalculateIndirectFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, irradiance);

		vec3 numerator = normal_distribution * geometry * fresnel;

		//Calculate the denominator.
		float outgoing_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(outgoing_direction, normal), 0.0f);
		float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(4.0f * outgoing_direction_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

		//Calculate the specular component.
		return vec3(numerator / denominator);
	}

	/*
	*	Calculates and returns the indirect normal distribution.
	*
	*	Calculated using the Trowbridge-Reitz GGX algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectNormalDistribution,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_FLOAT_EPSILON, 1.192092896e-07F);
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_PI, 3.141592f);

		//Calculate the roughness squared.
		float roughness_squared = roughness * roughness;

		//Calculate the numerator.
		float numerator = roughness_squared;

		//Calculate the denominator.
		float view_vector_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, outgoing_direction), 0.0f);
		float view_vector_coefficient_squared = view_vector_coefficient * view_vector_coefficient;
		float denominator_coefficient = view_vector_coefficient_squared * (roughness_squared - 1.0f) + 1.0f;

		float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_LIGHTING_PI * denominator_coefficient * denominator_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

		//Calculate the normal distribution.
		return vec3(numerator / denominator);
	}

	/*
	*	Calculates and returns the indirect geometry.
	*
	*	Calculated using the Schlick-GGX and Smith algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectGeometry,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_FLOAT_EPSILON, 1.192092896e-07F);

		//Calculate the outgoing direction coefficient.
		float outgoing_direction_coefficient = CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, outgoing_direction), 0.0f);

		//Calculate the roughness coefficient.
		float roughness_coefficient = (roughness * roughness) * 0.5f;

		//Calculate the first coefficient.
		float first_coefficient;

		{
			//Calculate the numerator.
			float numerator = outgoing_direction_coefficient;

			//Calculate the denominator.
			float denominator = CATALYST_SHADER_FUNCTION_MAXIMUM(outgoing_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient, CATALYST_LIGHTING_FLOAT_EPSILON);

			first_coefficient = numerator / denominator;
		}

		//Calculate the geometry.
		return vec3(first_coefficient);
	}

	/*
	*	Calculates and returns the indirect fresnel.
	*	
	*	Calculated using the cook-torrance algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectFresnel,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance)
	{
		//Calculate the surface color.
		vec3 surface_color = CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(vec3(0.04f, 0.04f, 0.04f), albedo, metallic);

		//Calculate the fresnel.
		return surface_color + CATALYST_SHADER_FUNCTION_MAXIMUM(vec3(1.0f, 1.0f, 1.0f) - surface_color, surface_color) * pow(1.0f - CATALYST_SHADER_FUNCTION_MAXIMUM(CATALYST_SHADER_FUNCTION_DOT_PRODUCT(outgoing_direction, normal), 0.0f), 5.0f);
	}

	/*
	*	Calculates and returns the indirect diffuse component.
	*
	*	Calculated using the Lambertian Diffuse algorithm.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectDiffuseComponent,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, CATALYST_LIGHTING_INVERSE_PI, 0.318309f);
		CATALYST_SHADER_CONSTANT(float, CATALIST_LIGHTING_INDIRECT_DIFFUSE_BOOST, 1.0f);

		//Calculate the diffuse component.
		return albedo * CATALYST_LIGHTING_INVERSE_PI * CATALIST_LIGHTING_INDIRECT_DIFFUSE_BOOST;
	}

	/*
	*	Calculates and returns the indirect specular weight.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SEVEN_ARGUMENTS(vec3,
													CalculateIndirectSpecularWeight,
													vec3 outgoing_direction,
													vec3 albedo,
													vec3 normal,
													float roughness,
													float metallic,
													float ambient_occlusion,
													vec3 irradiance)
	{
		//The fresnel is directly related to the specular weight, so use that.
		return CalculateIndirectFresnel(outgoing_direction, albedo, normal, roughness, metallic, ambient_occlusion, irradiance);
	}

CATALYST_SHADER_NAMESPACE_END()
#endif