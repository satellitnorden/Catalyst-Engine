//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) float BLOOM_THRESHOLD;
    layout (offset = 4) float BLOOM_INTENSITY;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SCENE_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Sample the scene texture.
	vec3 scene = texture(SCENE_TEXTURE, fragment_texture_coordinate).rgb;

	//Calculate the luminance.
	float luminance = CalculateAverage(scene);

    //Write the fragment.
    fragment = vec4(scene * max(luminance - BLOOM_THRESHOLD, 0.0f) * BLOOM_INTENSITY, 1.0f);
}