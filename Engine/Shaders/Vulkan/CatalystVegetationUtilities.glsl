#if !defined(CATALYST_VEGETATION_UTILITIES)
#define CATALYST_VEGETATION_UTILITIES

//Preprocessor defines.
#define MAXIMUM_WIND_SPEED (30.0f)
#define INVERSE_MAXIMUM_WIND_SPEED (1.0f / MAXIMUM_WIND_SPEED)
#define WIND_SPEED_TIME_MULTIPLIER (2.0f)
vec3 windDirection = vec3(1.0f, 0.0f, 1.0f);

/*
*	Calculates the wind displacement for vegetation.
*/
vec3 CalculateWindDisplacement(vec3 instance_position, vec3 vertex_position, vec3 vertex_normal, float time, float large_scale_wind_displacement_factor, float medium_scale_wind_displacement_factor, float small_scale_wind_displacement_factor)
{
	//Calculate the wind speed multiplier.
	float wind_speed_multiplier = windSpeed * INVERSE_MAXIMUM_WIND_SPEED;

	//Calculate the time factor.
	float time_factor = time * (1.0f + (wind_speed_multiplier * WIND_SPEED_TIME_MULTIPLIER));

    //Large scale motion.
    float large_scale_X = windDirection.x * (sin((instance_position.x + instance_position.y + instance_position.z + time_factor) * SQUARE_ROOT_OF_TWO ) + 1.0f);
    float large_scale_Z = windDirection.z * (cos((instance_position.x + instance_position.y + instance_position.z + time_factor) * HALF_PI) + 1.0f);

    large_scale_X *= large_scale_wind_displacement_factor;
    large_scale_Z *= large_scale_wind_displacement_factor;

    //Medium scale motion.
    float medium_scale_X = windDirection.x * (sin((instance_position.x + instance_position.y + instance_position.z + time_factor) * PHI) + 0.5f) * 0.25f;
    float medium_scale_Z = windDirection.z * (cos((instance_position.x + instance_position.y + instance_position.z + time_factor) * EULERS_NUMBER) + 0.5f) * 0.25f;

    medium_scale_X *= medium_scale_wind_displacement_factor;
    medium_scale_Z *= medium_scale_wind_displacement_factor;

    //Small scale motion.
    float small_scale_X = windDirection.x * sin((vertex_position.x + vertex_position.y + vertex_position.z + time_factor) * DOUBLE_PI) * 0.065f * vertex_normal.x;
    float small_scale_Y =                   cos((vertex_position.x + vertex_position.y + vertex_position.z + time_factor) * PI) * 0.065f * vertex_normal.y;
    float small_scale_Z = windDirection.z * sin((vertex_position.x + vertex_position.y + vertex_position.z + time_factor) * SQUARE_ROOT_OF_NINETY_NINE) * 0.065f * vertex_normal.z;

    small_scale_X *= small_scale_wind_displacement_factor;
    small_scale_Y *= medium_scale_wind_displacement_factor;
    small_scale_Z *= medium_scale_wind_displacement_factor;

    return vec3(large_scale_X + medium_scale_X + small_scale_X, small_scale_Y, large_scale_Z + medium_scale_Z + small_scale_Z) * wind_speed_multiplier;
}
#endif