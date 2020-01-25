#if !defined(CATALYST_OCEAN_CORE)
#define CATALYST_OCEAN_CORE

//Constants.
#define OCEAN_WAVE_HEIGHT (8.0f)
#define OCEAN_POSITION_SCALE (0.125f * 0.125f * 0.125f)
#define OCEAN_PERSISTENCE (0.5f)
#define OCEAN_LACUNARITY (2.0f)
#define BIAS_HEIGHT(X) (X * X * X * X)

/*
*   Samples the ocean height at the given point.
*/
float SampleOceanHeight(vec3 point, int ocean_texture_index)
{
    //Set up all variables.
    vec3 offset_1 = vec3(totalTime, 0.0f, totalTime) * SQUARE_ROOT_OF_TWO;
    vec3 offset_2 = vec3(totalTime, 0.0f, totalTime) * HALF_PI;
    vec3 offset_3 = vec3(totalTime, 0.0f, totalTime) * PHI;
    vec3 offset_4 = vec3(totalTime, 0.0f, totalTime) * EULERS_NUMBER;

    vec3 sample_point;
    float height_sample;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float total = 0.0f;
    float height = 0.0f;

    //Sample all levels.
    sample_point = ((point + offset_1) * frequency * SQUARE_ROOT_OF_TWO) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).x;
    height += BIAS_HEIGHT(height_sample) * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point + offset_2) * frequency * HALF_PI) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).y;
    height += BIAS_HEIGHT(height_sample) * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point + offset_3) * frequency * PHI) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).z;
    height += BIAS_HEIGHT(height_sample) * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    /*
    sample_point = ((point + offset_4) * frequency * EULERS_NUMBER) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).w;
    height += BIAS_HEIGHT(height_sample) * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;
	*/

    //Normalize the height.
    height /= total;

    return height * OCEAN_WAVE_HEIGHT;
}

#endif