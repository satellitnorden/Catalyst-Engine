#if !defined(CATALYST_ATMOSPHERIC_SCATTERING)
#define CATALYST_ATMOSPHERIC_SCATTERING

#define CATALYST_SHADER_LANGUAGE_CXX

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include "CatalystShaderCore.h"
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

CATALYST_SHADER_NAMESPACE_BEGIN(CatalystAtmosphericScattering)

	//Constants.
	#define CATALYST_ATMOSPHERIC_SCATTERING_MAXIMUM_ALTITUDE (16384.0f)
	#define CATALYST_ATMOSPHERIC_SCATTERING_MAXIMUM_ALTITUDE_RECIPROCAL (1.0f / CATALYST_ATMOSPHERIC_SCATTERING_MAXIMUM_ALTITUDE)
	#define SCATTERING_COEFFICIENTS (vec3(0.10662224073302790503956684714702f, 0.32444156446229323662271032653142f, 0.68301345536507069189263028481661f) * 16.0f)

	//Forward declarations.
	CATALYST_SHADER_FUNCTION_RETURN_FOUR_ARGUMENTS(vec3, CalculateAtmosphericScattering, vec3 ray_origin, vec3 ray_direction, vec4 sky_light_radiance, vec3 sky_light_direction);
	CATALYST_SHADER_FUNCTION_RETURN_TWO_ARGUMENTS(float, AtmospherePlaneIntersection, vec3 ray_origin, vec3 ray_direction);
	CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(float, CalculateDensityAtPosition, vec3 position);
	CATALYST_SHADER_FUNCTION_RETURN_THREE_ARGUMENTS(float, CalculateOpticalDepthAtPosition, vec3 ray_origin, vec3 ray_direction, float ray_distance);

	/*
	*	Calculates the atmospheric scattering.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_FOUR_ARGUMENTS(vec3, CalculateAtmosphericScattering, vec3 ray_origin, vec3 ray_direction, vec4 sky_light_radiance, vec3 sky_light_direction)
	{
		//Calculate the ray_distance.
		float ray_distance = AtmospherePlaneIntersection(ray_origin, ray_direction);

		float step_size = ray_distance * 0.25f;

		vec3 current_position = ray_origin;
		vec3 final_in_scattered_light = vec3(0.0f);

		for (uint i = 0; i < 4; ++i)
		{
			vec3 next_position = current_position + ray_direction * step_size;
			float sky_light_ray_distance = AtmospherePlaneIntersection(next_position, -sky_light_direction);
			float sky_light_ray_optical_depth = CalculateOpticalDepthAtPosition(current_position, -sky_light_direction, sky_light_ray_distance);
			float local_density = CalculateDensityAtPosition(current_position);
			float view_ray_optical_depth = CalculateOpticalDepthAtPosition(current_position, ray_direction, step_size);
			vec3 transmittance = exp(-(sky_light_ray_optical_depth + view_ray_optical_depth) * SCATTERING_COEFFICIENTS);

			final_in_scattered_light += local_density * transmittance;
			current_position = next_position;
		}

		final_in_scattered_light *= SCATTERING_COEFFICIENTS * step_size * CATALYST_ATMOSPHERIC_SCATTERING_MAXIMUM_ALTITUDE_RECIPROCAL * sky_light_radiance.rgb * (0.001f + (sky_light_radiance.a * 0.5f));

		return final_in_scattered_light;
	}

	/*
	*   Performs a ray/plane intersection.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_TWO_ARGUMENTS(float, AtmospherePlaneIntersection, vec3 ray_origin, vec3 ray_direction)
	{
		
		float ray_direction_plane_normal_dot_product = CATALYST_SHADER_FUNCTION_DOT_PRODUCT(ray_direction, vec3(0.0f, -1.0f, 0.0f));
		bool ray_direction_is_pointing_up = ray_direction_plane_normal_dot_product <= 0.0f;

		ray_direction_plane_normal_dot_product = ray_direction_plane_normal_dot_product > 0.0f ? CATALYST_SHADER_FUNCTION_MAXIMUM(ray_direction_plane_normal_dot_product, 0.0001f) : CATALYST_SHADER_FUNCTION_MINIMUM(ray_direction_plane_normal_dot_product, -0.0001f);

		float hit_distance = CATALYST_SHADER_FUNCTION_DOT_PRODUCT(vec3(0.0f, CATALYST_ATMOSPHERIC_SCATTERING_MAXIMUM_ALTITUDE, 0.0f) - ray_origin, vec3(0.0f, -1.0f, 0.0f)) / ray_direction_plane_normal_dot_product;

		return CATALYST_SHADER_FUNCTION_MINIMUM(hit_distance * float(ray_direction_is_pointing_up) + CATALYST_ATMOSPHERIC_SCATTERING_MAXIMUM_ALTITUDE * float(!ray_direction_is_pointing_up), CATALYST_ATMOSPHERIC_SCATTERING_MAXIMUM_ALTITUDE);
	}

	/*
	*	Returns the density at the given position.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(float, CalculateDensityAtPosition, vec3 position)
	{
		//Scale the height to [0.0f, 1.0f].
		float normalized_height = CATALYST_SHADER_FUNCTION_CLAMP(position.y * CATALYST_ATMOSPHERIC_SCATTERING_MAXIMUM_ALTITUDE_RECIPROCAL, 0.0f, 1.0f);

		//Calculate the density.
		return exp(-normalized_height * 16.0f) * (1.0f - normalized_height);
	}

	/*
	*	Returns the optical depth at the given position.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_THREE_ARGUMENTS(float, CalculateOpticalDepthAtPosition, vec3 ray_origin, vec3 ray_direction, float ray_distance)
	{
		vec3 current_position = ray_origin + ray_direction * (ray_distance * 0.25f);
		float current_optical_depth = 0.0f;

		for (uint i = 0; i < 4; ++i)
		{
			current_position += ray_direction * (ray_distance * 0.25f);
			current_optical_depth += CalculateDensityAtPosition(current_position);
		}

		return current_optical_depth;
	}

CATALYST_SHADER_NAMESPACE_END()

#endif