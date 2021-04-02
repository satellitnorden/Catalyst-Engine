#if !defined(CATALYST_CLOUDS_CORE)
#define CATALYST_CLOUDS_CORE

//Includes.
#include "CatalystGeometryMath.glsl"

//Constants.
#define CATALYST_CLOUDS_CLOUD_DENSITY_COORDINATE_SCALE (0.0000019073486328125f) //Equal to (1.0f / 524288.0f)
#define CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT (4096.0f)
#define CATALYST_CLOUDS_CLOUD_MAXIMUM_HEIGHT (8192.0f)
#define CATALYST_CLOUDS_MAXIMUM_RAY_DISTANCE (4096.0f)
#define CATALYST_CLOUDS_BASE_COLOR (vec3(0.8f, 0.9f, 1.0f))
#define CLOUD_DENSITY (0.250f) //0.025f step.
#define CATALYST_CLOUDS_LACUNARITY (5.25f) //0.25f step.
#define CATALYST_CLOUDS_GAIN (0.375f) //0.0025f step.

//The cloud texture.
layout (set = CLOUD_TEXTURE_SET_INDEX, binding = 0) uniform sampler3D CLOUD_TEXTURE;

/*
*	Returns the cloud density at the given world position.
*/
float GetCloudDensityAtWorldPosition(vec3 world_position)
{
	//Calculate the total density.
	float total_density = 0.0f;

	//Calculate the coordinate.
	vec3 coordinate = vec3(world_position.x * CATALYST_CLOUDS_CLOUD_DENSITY_COORDINATE_SCALE, world_position.y * CATALYST_CLOUDS_CLOUD_DENSITY_COORDINATE_SCALE, world_position.z * CATALYST_CLOUDS_CLOUD_DENSITY_COORDINATE_SCALE);
		
	//Add the four octaves.
	float total_amplitude = 0.0f;

	float current_frequency = 1.0f;
	float current_amplitude = 1.0f;

	total_density += texture(CLOUD_TEXTURE, coordinate * SQUARE_ROOT_OF_TWO)[0];

	total_amplitude += current_amplitude;

	current_frequency *= CATALYST_CLOUDS_LACUNARITY;
	current_amplitude *= CATALYST_CLOUDS_GAIN;

	total_density += texture(CLOUD_TEXTURE, coordinate * current_frequency * HALF_PI)[1] * current_amplitude;

	total_amplitude += current_amplitude;

	current_frequency *= CATALYST_CLOUDS_LACUNARITY;
	current_amplitude *= CATALYST_CLOUDS_GAIN;

	total_density += texture(CLOUD_TEXTURE, coordinate * current_frequency * PHI)[2] * current_amplitude;

	total_amplitude += current_amplitude;

	current_frequency *= CATALYST_CLOUDS_LACUNARITY;
	current_amplitude *= CATALYST_CLOUDS_GAIN;
	
	total_density += texture(CLOUD_TEXTURE, coordinate * current_frequency * EULERS_NUMBER)[3] * current_amplitude;

	total_amplitude += current_amplitude;

	current_frequency *= CATALYST_CLOUDS_LACUNARITY;
	current_amplitude *= CATALYST_CLOUDS_GAIN;
	

	//Normalize.
	total_density /= total_amplitude; //Equal to (1.0f / 1.875f)

	//Apply the cloud density.
	total_density = max((total_density - (1.0f - CLOUD_DENSITY)) * 4.75f, 0.0f);

	//Return the total density.
	return total_density;
}

/*
*	Returns the start and end positions.
*/
bool GetStartAndEndPositions(vec3 origin, vec3 direction, out vec3 start_position, out vec3 end_position)
{
	//Calculate the upwards factor.
	float upwards_factor = dot(direction, vec3(0.0f, 1.0f, 0.0f));

	//Is the origin beneath the lower plane?
	if (origin.y < CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT)
	{
		//Is the direction pointing upwards?
		if (upwards_factor > 0.0f)
		{
			//Construct the ray.
			Ray ray;

			ray._Origin = origin;
			ray._Direction = direction;

			//Construct the lower plane.
			Plane lower_plane;

			lower_plane._Position = vec3(0.0f, CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT, 0.0f);
			lower_plane._Normal = vec3(0.0f, -1.0f, 0.0f);

			//Calculate the start position.
			RayPlaneIntersectionResult lower_plane_result = RayPlaneIntersection(ray, lower_plane);
			start_position = origin + direction * lower_plane_result._HitDistance;

			//Modify the ray.
			ray._Origin = start_position;

			//Construct the upper plane.
			Plane upper_plane;

			upper_plane._Position = vec3(0.0f, CATALYST_CLOUDS_CLOUD_MAXIMUM_HEIGHT, 0.0f);
			upper_plane._Normal = vec3(0.0f, -1.0f, 0.0f);

			//Calculate the end position.
			RayPlaneIntersectionResult upper_plane_result = RayPlaneIntersection(ray, upper_plane);
			end_position = origin + direction * min(upper_plane_result._HitDistance, CATALYST_CLOUDS_MAXIMUM_RAY_DISTANCE);

			return true;
		}

		//Can't raycast. ):
		else
		{
			return false;
		}
	}

	//Is the origin on the lower plane?
	else if (origin.y == CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT)
	{
		//Is the direction pointing upwards?
		if (upwards_factor > 0.0f)
		{
			//Construct the ray.
			Ray ray;

			ray._Origin = origin;
			ray._Direction = direction;

			Plane upper_plane;

			upper_plane._Position = vec3(0.0f, CATALYST_CLOUDS_CLOUD_MAXIMUM_HEIGHT, 0.0f);
			upper_plane._Normal = vec3(0.0f, -1.0f, 0.0f);

			//Calculate the start and end positions.
			start_position = origin;

			RayPlaneIntersectionResult upper_plane_result = RayPlaneIntersection(ray, upper_plane);
			end_position = origin + direction * min(upper_plane_result._HitDistance, CATALYST_CLOUDS_MAXIMUM_RAY_DISTANCE);

			return true;
		}

		//Can't raycast. ):
		else
		{
			return false;
		}
	}

	//Is the origin beneath the upper plane (and above the lower plane)?
	else if (origin.y < CATALYST_CLOUDS_CLOUD_MAXIMUM_HEIGHT)
	{
		//Is the direction pointing upwards?
		if (upwards_factor > 0.0f)
		{
			//Construct the ray.
			Ray ray;

			ray._Origin = origin;
			ray._Direction = direction;

			Plane upper_plane;

			upper_plane._Position = vec3(0.0f, CATALYST_CLOUDS_CLOUD_MAXIMUM_HEIGHT, 0.0f);
			upper_plane._Normal = vec3(0.0f, -1.0f, 0.0f);

			//Calculate the start and end positions.
			start_position = origin;

			RayPlaneIntersectionResult upper_plane_result = RayPlaneIntersection(ray, upper_plane);
			end_position = origin + direction * min(upper_plane_result._HitDistance, CATALYST_CLOUDS_MAXIMUM_RAY_DISTANCE);

			return true;
		}

		//Is the direction pointing downwards?
		else if (upwards_factor < 0.0f)
		{
			//Construct the ray.
			Ray ray;

			ray._Origin = origin;
			ray._Direction = direction;

			Plane lower_plane;

			lower_plane._Position = vec3(0.0f, CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT, 0.0f);
			lower_plane._Normal = vec3(0.0f, 1.0f, 0.0f);

			//Calculate the start and end positions.
			start_position = origin;

			RayPlaneIntersectionResult lower_plane_result = RayPlaneIntersection(ray, lower_plane);
			end_position = origin + direction * min(lower_plane_result._HitDistance, CATALYST_CLOUDS_MAXIMUM_RAY_DISTANCE);

			return true;
		}

		//Is the direction pointing straigh ahead?
		else if (upwards_factor == 0.0f)
		{
			//Calculate the start and end positions.
			start_position = origin;
			end_position = origin + direction * CATALYST_CLOUDS_MAXIMUM_RAY_DISTANCE;

			return true;
		}
	}

	//Is the origin on the upper plane (and above the lower plane)?
	else if (origin.y == CATALYST_CLOUDS_CLOUD_MAXIMUM_HEIGHT)
	{
		//Is the direction pointing downwards?
		if (upwards_factor < 0.0f)
		{
			//Construct the ray.
			Ray ray;

			ray._Origin = origin;
			ray._Direction = direction;

			Plane lower_plane;

			lower_plane._Position = vec3(0.0f, CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT, 0.0f);
			lower_plane._Normal = vec3(0.0f, 1.0f, 0.0f);

			//Calculate the start and end positions.
			start_position = origin;

			RayPlaneIntersectionResult lower_plane_result = RayPlaneIntersection(ray, lower_plane);
			end_position = origin + direction * min(lower_plane_result._HitDistance, CATALYST_CLOUDS_MAXIMUM_RAY_DISTANCE);

			return true;
		}

		//Can't raycast. ):
		else
		{
			return false;
		}
	}

	//Can't raycast. ):
	else
	{
		return false;
	}
}

/*
*	Returns the cloud density from the given world position in the given direction.
*	Also requires some extra arguments to be able to calculate the density.
*/
float GetCloudDensityInDirection(vec3 world_position, vec3 direction, uvec2 blue_noise_coordinate)
{
	//Calculate the start and end positions.
	vec3 start_position;
	vec3 end_position;

	if (!GetStartAndEndPositions(world_position, direction, start_position, end_position))
	{
		return 0.0f;
	}

	else
	{
		//Sample the blue noise.
		vec4 blue_noise = SampleBlueNoiseTexture(blue_noise_coordinate, 0);

		//Calculate the total density.
		float total_density = 0.0f;

		for (uint i = 0; i < 4; ++i)
		{
			total_density += GetCloudDensityAtWorldPosition(mix(start_position, end_position, float(i) * 0.25f + blue_noise[i] * 0.25f));
		}

		//Return the total density.
		return min(total_density, 1.0f);
	}
}

/*
*	Returns the cloud color from the given world position in the given direction.
*	Also requires some extra arguments to be able to calculate the density.
*/
vec4 GetCloudColorInDirection(vec3 world_position, vec3 direction, uvec2 blue_noise_coordinate, vec3 sun_direction, vec3 sun_radiance)
{
	//Calculate the start and end positions.
	vec3 start_position;
	vec3 end_position;

	if (!GetStartAndEndPositions(world_position, direction, start_position, end_position))
	{
		return vec4(0.0f);
	}

	else
	{
		//Sample the blue noise.
		vec4 blue_noise = SampleBlueNoiseTexture(blue_noise_coordinate, 0);

		//Sample the lower and upper sky color.
		vec3 lower_sky_color = SampleSky(vec3(0.0f, -1.0f, 0.0f), 0.0f);
		vec3 upper_sky_color = SampleSky(vec3(0.0f, 1.0f, 0.0f), 0.0f);

		//Calculate the total density.
		vec3 final_color = vec3(0.0f, 0.0f, 0.0f);
		float total_density = 0.0f;

		for (uint i = 0; i < 4; ++i)
		{
			//Calculate the position.
			vec3 position = mix(start_position, end_position, float(i) * 0.25f + blue_noise[i] * 0.25f);

			//Calculate the density.
			float density = GetCloudDensityAtWorldPosition(position);

			//Remember the old total density.
			float old_total_density = total_density;

			//Add the the total density.
			total_density = min(total_density + density, 1.0f);

			//Calculate the new density.
			float new_density = density - (total_density - (old_total_density + density));

			//Calculate the ambient factor.
			float ambient_factor = clamp(position.y - CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT, 0.0f, CATALYST_CLOUDS_CLOUD_MAXIMUM_HEIGHT - CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT) / (CATALYST_CLOUDS_CLOUD_MAXIMUM_HEIGHT - CATALYST_CLOUDS_CLOUD_MINIMUM_HEIGHT);

			//Add the ambient lighting to the final color.
			//final_color += CATALYST_CLOUDS_BASE_COLOR * mix(lower_sky_color, upper_sky_color, ambient_factor) * new_density;

			//Add the sun lighting.
			float density_in_sun_direction = GetCloudDensityInDirection(position, sun_direction, blue_noise_coordinate);

			final_color += CATALYST_CLOUDS_BASE_COLOR * sun_radiance * (1.0f - density_in_sun_direction) * new_density;
		}

		//Return the total density.
		return vec4(final_color, total_density);
	}
}

#endif