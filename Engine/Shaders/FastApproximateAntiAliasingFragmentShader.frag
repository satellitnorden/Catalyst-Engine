//FIXED THRESHOLD
// Trims the algorithm from processing darks.
//   0.0833 - upper limit (default, the start of visible unfiltered edges)
//   0.0625 - high quality (faster)
//   0.0312 - visible limit (slower)

// RELATIVE THRESHOLD
// The minimum amount of local contrast required to apply algorithm.
//   0.333 - too little (faster)
//   0.250 - low quality
//   0.166 - default
//   0.125 - high quality 
//   0.063 - overkill (slower)

//Constants.
#define FXAA_FIXED_THRESHOLD (0.0312f)
#define FXAA_RELATIVE_THRESHOLD (0.063f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) uint PASSTHROUGH;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SCENE_TEXTURE_NEAREST;
layout (set = 1, binding = 1) uniform sampler2D SCENE_TEXTURE_LINEAR;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*   Returns if FXAA can be skipped.
*/
bool CanSkipFXAA(float luminance_range, float maximum_luminance)
{
    return luminance_range < max(FXAA_FIXED_THRESHOLD, maximum_luminance * FXAA_RELATIVE_THRESHOLD);
}

void CatalystShaderMain()
{
    //Sample the scene texture.
    vec4 middle_sample = texture(SCENE_TEXTURE_NEAREST, fragment_texture_coordinate);

    if (PASSTHROUGH == 1)
    {
        //Write the fragment
        scene = vec4(vec3(middle_sample.rgb), 1.0f);
    }
    
    else
    {
        //Sample the neighbors to the left, right, down and up.
        vec4 left_sample = texture(SCENE_TEXTURE_NEAREST, fragment_texture_coordinate + vec2(-1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION);
        vec4 right_sample = texture(SCENE_TEXTURE_NEAREST, fragment_texture_coordinate + vec2(1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION);
        vec4 down_sample = texture(SCENE_TEXTURE_NEAREST, fragment_texture_coordinate + vec2(0.0f, -1.0f) * INVERSE_SCALED_RESOLUTION);
        vec4 up_sample = texture(SCENE_TEXTURE_NEAREST, fragment_texture_coordinate + vec2(0.0f, 1.0f) * INVERSE_SCALED_RESOLUTION);

        //Calculate the minimum/maximum luminance.
        float minimum_luminance = min(min(left_sample.a, right_sample.a), min(down_sample.a, up_sample.a));
        float maximum_luminance = max(max(left_sample.a, right_sample.a), max(down_sample.a, up_sample.a));

        //Calculate the luminance range.
        float luminance_range = maximum_luminance - minimum_luminance;

        //Should FXAA be skipped?
        if (CanSkipFXAA(luminance_range, maximum_luminance))
        {
            //Write the fragment.
            scene = middle_sample;
        }

        else
        {
            //Calculate the blend factor.
            float blend_factor = (left_sample.a + right_sample.a + down_sample.a + right_sample.a) * 0.25f;
            blend_factor = abs(blend_factor - middle_sample.a);
            blend_factor /= luminance_range;
            blend_factor = smoothstep(0.0f, 1.0f, blend_factor);
            blend_factor *= blend_factor;

            //Determine if the blend should happen over the horizontal edge.
            bool is_horizontal_edge = abs(up_sample.a + down_sample.a - 2.0f * middle_sample.a) >= abs(right_sample.a + left_sample.a - 2.0f * middle_sample.a);

            //Calculate the gradients.
            float positive_gradient = up_sample.a * float(is_horizontal_edge) + right_sample.a * float(!is_horizontal_edge);
            float negative_gradient = down_sample.a * float(is_horizontal_edge) + left_sample.a * float(!is_horizontal_edge);

            positive_gradient = abs(positive_gradient - middle_sample.a);
            negative_gradient = abs(negative_gradient - middle_sample.a);

            //Calculate the pixel step.
            vec2 pixel_step = ((vec2(0.0f, 1.0f) * float(is_horizontal_edge) + vec2(1.0f, 0.0f) * float(!is_horizontal_edge)) * float(positive_gradient >= negative_gradient))
                                + ((vec2(0.0f, -1.0f) * float(is_horizontal_edge) + vec2(-1.0f, 0.0f) * float(!is_horizontal_edge)) * float(positive_gradient < negative_gradient));
            pixel_step *= INVERSE_SCALED_RESOLUTION;

            //Finally, perform the blend!
            vec4 final_blend = texture(SCENE_TEXTURE_LINEAR, fragment_texture_coordinate + pixel_step * blend_factor);

            //Write the fragment!
            scene = final_blend;
        }
    }
}