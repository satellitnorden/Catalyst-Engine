//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 INVERSE_RESOLUTION;
    layout (offset = 8) uint MIP_INDEX;
};

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SOURCE_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
    if (MIP_INDEX == 0)
    {
        fragment = vec4(texture(SOURCE_TEXTURE, fragment_texture_coordinate).w);
    }
	
    else
    {
        //Calculate the texel offset.
        vec2 texel_offset = INVERSE_RESOLUTION;

        //Compare the depths in the scene features 2 texture. Choose the texel for the downsample with the depth closest to the camera.
        float source_sample_1 = texture(SOURCE_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, -texel_offset.y)).x;
        float source_sample_2 = texture(SOURCE_TEXTURE, fragment_texture_coordinate + vec2(-texel_offset.x, texel_offset.y)).x;
        float source_sample_3 = texture(SOURCE_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, -texel_offset.y)).x;
        float source_sample_4 = texture(SOURCE_TEXTURE, fragment_texture_coordinate + vec2(texel_offset.x, texel_offset.y)).x;

        //Write the fragment.
        fragment = vec4(max(max(source_sample_1, source_sample_2), max(source_sample_3, source_sample_4)));
    }
}