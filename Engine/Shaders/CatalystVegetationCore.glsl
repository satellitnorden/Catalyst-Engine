#if !defined(CATALYST_VEGETATION_CORE)
#define CATALYST_VEGETATION_CORE

//Preprocessor defines.
#define MAXIMUM_WIND_SPEED (30.0f)
#define INVERSE_MAXIMUM_WIND_SPEED (1.0f / MAXIMUM_WIND_SPEED)
#define WIND_DIRECTION (vec3(1.0f, 0.0f, 0.0f))

/*
*	Calculates the wind displacement for vegetation.
*/
vec3 CalculateWindDisplacement(vec3 instance_position, vec3 vertex_position, vec3 vertex_normal, float large_scale_wind_displacement_factor, float medium_scale_wind_displacement_factor, float small_scale_wind_displacement_factor)
{
	//Calculate the wind speed multiplier.
	float wind_speed_multiplier = 10.0f * INVERSE_MAXIMUM_WIND_SPEED;

	//Calculate the time factor.
	float time_factor = totalTime * (1.0f + wind_speed_multiplier);

    //Large scale motion.
    float large_scale_X = WIND_DIRECTION.x * (sin((instance_position.x + instance_position.y + instance_position.z + time_factor) * SQUARE_ROOT_OF_TWO ) + 1.0f);
    float large_scale_Z = WIND_DIRECTION.z * (cos((instance_position.x + instance_position.y + instance_position.z + time_factor) * HALF_PI) + 1.0f);

    large_scale_X *= large_scale_wind_displacement_factor;
    large_scale_Z *= large_scale_wind_displacement_factor;

    //Medium scale motion.
    float medium_scale_X = WIND_DIRECTION.x * (sin((instance_position.x + instance_position.y + instance_position.z + time_factor) * EULERS_NUMBER) + 0.5f) * 0.5f;
    float medium_scale_Z = WIND_DIRECTION.z * (cos((instance_position.x + instance_position.y + instance_position.z + time_factor) * PHI) + 0.5f) * 0.5f;

    medium_scale_X *= medium_scale_wind_displacement_factor;
    medium_scale_Z *= medium_scale_wind_displacement_factor;

    //Small scale motion.
    float small_scale_X = WIND_DIRECTION.x * sin((vertex_position.x + vertex_position.y + vertex_position.z + time_factor) * DOUBLE_PI) * 0.25f * vertex_normal.x;
    float small_scale_Y =                   cos((vertex_position.x + vertex_position.y + vertex_position.z + time_factor) * PI) * 0.25f * vertex_normal.y;
    float small_scale_Z = WIND_DIRECTION.z * sin((vertex_position.x + vertex_position.y + vertex_position.z + time_factor) * DOUBLE_PI) * 0.25f * vertex_normal.z;

    small_scale_X *= small_scale_wind_displacement_factor;
    small_scale_Y *= small_scale_wind_displacement_factor;
    small_scale_Z *= small_scale_wind_displacement_factor;

    return vec3(large_scale_X + medium_scale_X + small_scale_X, small_scale_Y, large_scale_Z + medium_scale_Z + small_scale_Z) * wind_speed_multiplier;
}
#endif