#if !defined(CATALYST_WIND_CORE)
#define CATALYST_WIND_CORE

//Global uniform data.
layout (std140, set = 1, binding = 0) uniform WindDynamicUniformData
{
    layout (offset = 0) vec4 WIND_DATA;
};

/*
*	Calculates the wind displacement for vegetation.
*/
vec3 CalculateWindDisplacement(vec3 instance_position, vec3 vertex_position, vec3 vertex_normal, float large_scale_wind_displacement_factor, float medium_scale_wind_displacement_factor, float small_scale_wind_displacement_factor)
{
    //Large scale motion.
    float large_scale_X = WIND_DATA.x * (sin((instance_position.x + instance_position.y + instance_position.z + (totalTime * WIND_DATA[3])) * SQUARE_ROOT_OF_TWO ) + 0.75f);
    float large_scale_Z = WIND_DATA.z * (cos((instance_position.x + instance_position.y + instance_position.z + (totalTime * WIND_DATA[3])) * HALF_PI) + 0.75f);

    large_scale_X *= large_scale_wind_displacement_factor;
    large_scale_Z *= large_scale_wind_displacement_factor;

    //Medium scale motion.
    float medium_scale_X = WIND_DATA.x * (sin((instance_position.x + instance_position.y + instance_position.z + (totalTime * WIND_DATA[3])) * EULERS_NUMBER) + 0.5f) * 0.5f;
    float medium_scale_Z = WIND_DATA.z * (cos((instance_position.x + instance_position.y + instance_position.z + (totalTime * WIND_DATA[3])) * PHI) + 0.5f) * 0.5f;

    medium_scale_X *= medium_scale_wind_displacement_factor;
    medium_scale_Z *= medium_scale_wind_displacement_factor;

    //Small scale motion.
    float small_scale_X = WIND_DATA.x * (sin((vertex_position.x + vertex_position.y + vertex_position.z + (totalTime * WIND_DATA[3])) * DOUBLE_PI) + 0.25f) * 0.25f * vertex_normal.x;
    float small_scale_Y =               (cos((vertex_position.x + vertex_position.y + vertex_position.z + (totalTime * WIND_DATA[3])) * PI) + 0.25f) * 0.25f * vertex_normal.y;
    float small_scale_Z = WIND_DATA.z * (sin((vertex_position.x + vertex_position.y + vertex_position.z + (totalTime * WIND_DATA[3])) * DOUBLE_PI) + 0.25f) * 0.25f * vertex_normal.z;

    small_scale_X *= small_scale_wind_displacement_factor;
    small_scale_Y *= small_scale_wind_displacement_factor;
    small_scale_Z *= small_scale_wind_displacement_factor;

    return vec3(large_scale_X + medium_scale_X + small_scale_X, small_scale_Y, large_scale_Z + medium_scale_Z + small_scale_Z) * (WIND_DATA[3] * 0.1f);
}
#endif