//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Applies fast approximate anti aliasing
*/
vec3 ApplyFastApproximateAntiAliasing(vec3 fragment)
{
    //Calculate the anti aliased fragment.
    vec3 anti_aliased_fragment = fragment;
    float anti_aliased_fragment_weight = 1.0f;

    //Calculate the average of this fragment.
    float average = CalculateAverage(fragment);

    //Sample the neighboring pixels.
    vec3 indirect_samples[4] = vec3[]
    (  
        texture(scene_texture, fragment_texture_coordinate + vec2(-1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION).rgb,
        texture(scene_texture, fragment_texture_coordinate + vec2(-1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION).rgb,
        texture(scene_texture, fragment_texture_coordinate + vec2(1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION).rgb,
        texture(scene_texture, fragment_texture_coordinate + vec2(1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION).rgb
    );

    for (int i = 0; i < 4; ++i)
    {
        //Calculate the average for the sample.
        float sample_average = CalculateAverage(indirect_samples[i]);

        //Calculate the weight.
        float weight = abs(average - sample_average) * 0.125f;

        //Blend in the sample based on the weight.
        anti_aliased_fragment += indirect_samples[i] * weight;
        anti_aliased_fragment_weight += weight;
    }

    vec3 straight_samples[4] = vec3[]
    (  
        texture(scene_texture, fragment_texture_coordinate + vec2(-1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION).rgb,
        texture(scene_texture, fragment_texture_coordinate + vec2(1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION).rgb,
        texture(scene_texture, fragment_texture_coordinate + vec2(0.0f, -1.0f) * INVERSE_SCALED_RESOLUTION).rgb,
        texture(scene_texture, fragment_texture_coordinate + vec2(0.0f, 1.0f) * INVERSE_SCALED_RESOLUTION).rgb
    );

    for (int i = 0; i < 4; ++i)
    {
        //Calculate the average for the sample.
        float sample_average = CalculateAverage(straight_samples[i]);

        //Calculate the weight.
        float weight = abs(average - sample_average) * 0.25f;

        //Blend in the sample based on the weight.
        anti_aliased_fragment += straight_samples[i] * weight;
        anti_aliased_fragment_weight += weight;
    }

    //Normalize the anti aliased fragment.
    anti_aliased_fragment /= anti_aliased_fragment_weight;

    //Return the anti aliased fragment.
    return anti_aliased_fragment;
}

void CatalystShaderMain()
{
    //Sample the scene.
    vec3 scene = texture(scene_texture, fragment_texture_coordinate).rgb;

    //Apply fast approximate anti aliasing.
    scene = ApplyFastApproximateAntiAliasing(scene);

    //Write the fragment.
    fragment = vec4(scene, 1.0f);
} 